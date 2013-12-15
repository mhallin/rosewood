#import "RWBuildController.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#import <Python.h>

static const float kAnimationBarHeight = 6;

static void file_stream_callback(__unused ConstFSEventStreamRef streamRef,
                                 void *clientCallBackInfo,
                                 size_t numEvents,
                                 void *eventPaths,
                                 __unused const FSEventStreamEventFlags eventFlags[],
                                 __unused const FSEventStreamEventId eventIds[]) {
    RWBuildController *delegate = (__bridge RWBuildController*)clientCallBackInfo;

    for (size_t i = 0; i < numEvents; ++i) {
        const char *path = ((const char**)eventPaths)[i];

        [delegate notifyFileChanged:@(path)];
    }
}

static void tick_connection_callback(CFRunLoopObserverRef __unused observer,
                                     CFRunLoopActivity __unused activity,
                                     void *info) {
    RWBuildController *delegate = (__bridge RWBuildController*)info;

    [delegate tickConnection];
}

@interface RWBuildController () <NSOpenSavePanelDelegate, NSNetServiceDelegate, NSNetServiceBrowserDelegate> {
    BOOL _working;

    NSStatusItem *_statusItem;
    double _statusAnimationCounter;
    NSTimer *_animationTimer;

    NSOperationQueue *_builderQueue;

    NSString *_projectRoot;

    FSEventStreamRef _fsEventStream;

    NSNetServiceBrowser *_netServiceBrowser;
    NSMutableArray *_detectedServices;

    id<RWDeviceConnection> _connection;
    CFRunLoopObserverRef _connectionTicker;
}

@property (strong) IBOutlet NSMenu *statusMenu;
@property (strong) IBOutlet NSMenuItem *devicesMenu;
@property (strong) IBOutlet NSMenuItem *connectionStatusItem;

- (IBAction)forceProjectRescan:(id)sender;

@end

@implementation RWBuildController

- (void)dealloc {
    FSEventStreamStop(_fsEventStream);
    FSEventStreamUnscheduleFromRunLoop(_fsEventStream,
                                       CFRunLoopGetCurrent(),
                                       kCFRunLoopCommonModes);
    FSEventStreamInvalidate(_fsEventStream);
    FSEventStreamRelease(_fsEventStream);

    CFRunLoopRemoveObserver(CFRunLoopGetCurrent(),
                            _connectionTicker,
                            kCFRunLoopCommonModes);
    CFRelease(_connectionTicker);
}

- (void)notifyFileChanged:(NSString *)url {
    [_builderQueue addOperation:[_worker rebuildFileOperation:url]];
}

- (void)tickConnection {
    [_connection tickLoop];
}

- (void)setWorking:(BOOL)working {
    if (_working == working) return;

    if (_working) {
        [_animationTimer invalidate];
        _animationTimer = nil;
    }

    _working = working;

    if (_working) {
        _statusAnimationCounter = -kAnimationBarHeight;
        _animationTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                                           target:self
                                                         selector:@selector(animationTimerTick:)
                                                         userInfo:nil
                                                          repeats:YES];
    }
}

#pragma mark - Application Delegate Methods

- (void)applicationDidFinishLaunching:(NSNotification *)__unused notification {
    [self showProjectDirectoryDialog:self];

    if (!_projectRoot) {
        [[NSApplication sharedApplication] terminate:self];
    }

    [self createBuilderQueue];
    [self startFileChangeListener];
    [self startNetServiceBrowser];
    [self startRunLoopObserver];

    [self createStatusBarItem];

    [self setupPythonWorker];
}

#pragma mark - KVO

- (void)observeValueForKeyPath:(NSString *)keyPath
ofObject:(id)__unused object
change:(NSDictionary *)__unused change
context:(void *)__unused context {
    if ([keyPath isEqualToString:@"operationCount"] || [keyPath isEqualToString:@"queueSize"]) {
        long count = _builderQueue.operationCount;
        int queueSize = [_connection queueSize].intValue;
        BOOL working = count || queueSize;

        dispatch_async(dispatch_get_main_queue(), ^{
            [self setWorking:working];
            [self updateStatusBarItem];
        });
    }
}

#pragma mark - IB Actions

- (IBAction)showProjectDirectoryDialog:(id)__unused sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    panel.title = @"Choose Project Root";
    panel.delegate = self;
    panel.canChooseDirectories = YES;
    panel.canChooseFiles = NO;
    panel.allowsMultipleSelection = NO;
    NSInteger result = [panel runModal];

    if (result != NSFileHandlingPanelOKButton) return;

    NSLog(@"Project directory: %@", panel.URL);

    _projectRoot = panel.URL.path;
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setURL:panel.URL forKey:@"projectRoot"];
    [defaults synchronize];
}

- (IBAction)forceProjectRescan:(id)__unused sender {
    [_builderQueue addOperation:[_worker constructBuildGraphOperation]];
}

#pragma mark - Animations (drawing)

- (void)animationTimerTick:(id)__unused sender {
    _statusAnimationCounter += 0.2;
    [self updateStatusBarItem];
}

- (void)updateStatusBarItem {
    NSImage *image = [NSImage imageWithSize:NSMakeSize(11, 17) flipped:NO drawingHandler:^BOOL(NSRect dstRect) {

        dstRect.origin.x += 1;
        dstRect.size.width -= 2;

        NSBezierPath *rhombus = [self drawRhombus:dstRect];

        NSColor *baseColor = (_connection
                              ? [NSColor blackColor]
                              : [NSColor colorWithCalibratedWhite:0.0 alpha:0.35]);

        [baseColor setStroke];
        [rhombus stroke];

        if (_working) {
            [baseColor setFill];

            NSRect clipRect = NSMakeRect(0, _statusAnimationCounter,
                                         dstRect.size.width, kAnimationBarHeight);
            NSBezierPath *clipPath = [NSBezierPath bezierPathWithRect:clipRect];

            [clipPath setClip];
            [rhombus fill];

            if (_statusAnimationCounter + kAnimationBarHeight > dstRect.size.height - 2) {
                clipRect.origin.y -= dstRect.size.height - 2;
                clipPath = [NSBezierPath bezierPathWithRect:clipRect];
                [clipPath setClip];
                [rhombus fill];

                if (_statusAnimationCounter > dstRect.size.height + kAnimationBarHeight) {
                    _statusAnimationCounter -= dstRect.size.height - 2;
                }
            }
        }

        return YES;
    }];

    _statusItem.image = image;
}

- (NSBezierPath *)drawRhombus:(NSRect)rect {
    NSBezierPath *path = [NSBezierPath bezierPath];

    double x = rect.origin.x;
    double y = rect.origin.y;
    double w = rect.size.width;
    double h = rect.size.height;

    [path moveToPoint:NSMakePoint(x + w/2, y)];
    [path lineToPoint:NSMakePoint(x, y + h/2)];
    [path lineToPoint:NSMakePoint(x + w/2, y + h)];
    [path lineToPoint:NSMakePoint(x + w, y + h/2)];
    [path closePath];

    return path;
}

#pragma mark - Python Support

- (void)setupPython {
    NSArray *possibleMainExtensions = @[ @"py", @"pyc", @"pyo" ];
    NSString *mainFilePath = nil;

    for (NSString *possibleMainExtension in possibleMainExtensions) {
        mainFilePath = [[NSBundle mainBundle] pathForResource:@"main"
                                                       ofType:possibleMainExtension];
        if (mainFilePath) break;
    }

    if (!mainFilePath) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"Failed to find the Main.{py,pyc,pyo} file in the application wrapper's Resources directory."];
    }

    const char *mainFilePathPtr = [mainFilePath UTF8String];
    FILE *mainFile = fopen(mainFilePathPtr, "r");
    int result = PyRun_SimpleFile(mainFile, (char *)[[mainFilePath lastPathComponent] UTF8String]);

    if (result != 0) {
        [NSException raise:NSInternalInconsistencyException
                    format:@"PyRun_SimpleFile failed with file '%@'0", mainFilePath];
    }
}

- (void)setupPythonWorker {
    [self setupPython];
    if (!_worker) {
        NSLog(@"No worker created by python!");
    }

    [_worker setProjectRoot:_projectRoot];
    [_worker setDelegate:self];
    PyEval_SaveThread();
    [_builderQueue addOperation:[_worker constructBuildGraphOperation]];
}

#pragma mark - Initialization

- (void)createBuilderQueue {
    _builderQueue = [[NSOperationQueue alloc] init];
    _builderQueue.maxConcurrentOperationCount = 1;
    [_builderQueue addObserver:self
                    forKeyPath:@"operationCount"
                       options:NSKeyValueObservingOptionNew
                       context:NULL];
}

- (void)createStatusBarItem {
    _statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:30];
    _statusItem.menu = _statusMenu;
    _statusItem.highlightMode = YES;

    [self updateStatusBarItem];
}

- (void)startFileChangeListener {
    NSString *assetDir = [_projectRoot stringByAppendingPathComponent:@"game-assets"];
    CFArrayRef paths = (__bridge_retained CFArrayRef) @[assetDir];

    FSEventStreamContext context = {0, (__bridge void*)self, NULL, NULL, NULL};

    _fsEventStream = FSEventStreamCreate(kCFAllocatorDefault,
                                         file_stream_callback,
                                         &context,
                                         paths,
                                         kFSEventStreamEventIdSinceNow,
                                         3,
                                         kFSEventStreamCreateFlagFileEvents);

    FSEventStreamScheduleWithRunLoop(_fsEventStream,
                                     CFRunLoopGetCurrent(),
                                     kCFRunLoopCommonModes);
    FSEventStreamStart(_fsEventStream);
}

- (void)startNetServiceBrowser {
    _detectedServices = [NSMutableArray array];
    _netServiceBrowser = [[NSNetServiceBrowser alloc] init];
    _netServiceBrowser.delegate = self;
    [_netServiceBrowser searchForServicesOfType:@"_iteratool._tcp" inDomain:@""];
}

- (void)startRunLoopObserver {
    CFRunLoopObserverContext context = {0, (__bridge void*)self, NULL, NULL, NULL};

    _connectionTicker = CFRunLoopObserverCreate(kCFAllocatorDefault,
                                                kCFRunLoopBeforeSources,
                                                YES,
                                                0,
                                                tick_connection_callback,
                                                &context);

    CFRunLoopAddObserver(CFRunLoopGetCurrent(),
                         _connectionTicker,
                         kCFRunLoopCommonModes);
}

#pragma mark - Build Worker Delegate

- (void)worker:(id<RWBuildWorker>)worker reportsSourceFileChanged:(NSString *)sourcePath {
    [_builderQueue addOperation:[worker rebuildFileOperation:sourcePath]];
}

- (void)worker:(id<RWBuildWorker>)__unused worker reportsBuildSuccessFromSources:(NSArray *)__unused sourcePaths toDestinations:(NSArray *)destinationPaths {
    dispatch_async(dispatch_get_main_queue(), ^{
        for (NSString *dest in destinationPaths) {
            NSUserNotification *notification = [[NSUserNotification alloc] init];
            notification.title = @"Asset Build Successful";
            notification.subtitle = [NSString stringWithFormat:@"Built %@", dest.pathComponents.lastObject];

            NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
            [center deliverNotification:notification];

            for (NSString *path in destinationPaths) {
                [self sendAsset:path];
            }
        }
    });
}

- (void)worker:(id<RWBuildWorker>)__unused worker reportsBuildFailureFromSources:(NSString *)__unused sourcePaths toDestinations:(NSArray *)destinationPaths {
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *dest = destinationPaths[0];
        NSUserNotification *notification = [[NSUserNotification alloc] init];
        notification.title = @"Asset Build Failed";
        notification.subtitle = [NSString stringWithFormat:@"Could not build %@", dest.pathComponents.lastObject];

        NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
        [center deliverNotification:notification];
    });
}

- (void)worker:(id<RWBuildWorker>)__unused worker needsToExecuteOperation:(NSOperation *)operation {
    [_builderQueue addOperation:operation];
}

#pragma mark - Net Service Browser Delegate and related

- (void)netServiceBrowser:(NSNetServiceBrowser *)__unused aNetServiceBrowser
didFindService:(NSNetService *)aNetService
moreComing:(BOOL)moreComing {
    [_detectedServices addObject:aNetService];

    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.title = @"Found Iteratool";
    notification.subtitle = [NSString stringWithFormat:@"Running on %@", aNetService.name];

    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    [center deliverNotification:notification];
    if (!moreComing) [self rebuildServiceMenu];
}

- (void)netServiceBrowser:(NSNetServiceBrowser *)__unused aNetServiceBrowser
didRemoveService:(NSNetService *)aNetService
moreComing:(BOOL)moreComing {
    [_detectedServices removeObject:aNetService];
    if (!moreComing) [self rebuildServiceMenu];
}

- (void)rebuildServiceMenu {
    [_devicesMenu.submenu removeAllItems];
    for (NSNetService *service in _detectedServices) {
        NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:service.name
                                                      action:@selector(connectToService:) keyEquivalent:@""];
        item.tag = [_detectedServices indexOfObject:service];
        [_devicesMenu.submenu addItem:item];
    }
}

- (void)connectToService:(NSMenuItem *)item {
    NSNetService *service = [_detectedServices objectAtIndex:item.tag];
    service.delegate = self;
    [service resolveWithTimeout:5];
}

#pragma mark - Net Service Delegate and connection related

- (void)netService:(NSNetService *)__unused sender didNotResolve:(NSDictionary *)errorDict {
    NSLog(@"Could not resolve service: %@", errorDict);
}

- (void)netServiceDidResolveAddress:(NSNetService *)sender {
    NSString *hostname;
    NSNumber *port;

    for (NSData *data in sender.addresses) {
        const struct sockaddr_in *addr = data.bytes;
        if (addr->sin_family == AF_INET) {
            char addr_str[20] = {0};
            inet_ntop(AF_INET, &addr->sin_addr, addr_str, sizeof addr_str);
            hostname = @(addr_str);
            port = @(ntohs(addr->sin_port));
            break;
        }
    }

    if (!hostname) {
        NSLog(@"Did not find an IPv4 address for the service");
        return;
    }

    [_connection disconnect];
    [((id)_connection) removeObserver:self
                           forKeyPath:@"queueSize"];

    _connection = [_connectionFactory createConnectionToHost:hostname port:port];
    [((id)_connection) addObserver:self
                        forKeyPath:@"queueSize"
                           options:NSKeyValueObservingOptionNew
                           context:NULL];
    [_connection appendCommand:@[@"get_asset", @"rw_build.cache"]];

    _connectionStatusItem.title = [NSString stringWithFormat:@"Connected to %@", sender.name];
}

- (void)sendAsset:(NSString *)path {
    NSData *data = [NSData dataWithContentsOfFile:path];
    NSString *relative = [path substringFromIndex:_projectRoot.length + @"/asset-build/".length];

    if (data) {
        [_connection appendCommand:@[@"reload_asset", relative, data]];
    }
    else {
        NSLog(@"WARNING: No data at path: %@", data);
    }
}

@end
