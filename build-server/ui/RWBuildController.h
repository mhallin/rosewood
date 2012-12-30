#import <Cocoa/Cocoa.h>

#import "../build/RWBuildWorker.h"
#import "../remote/RWDeviceConnection.h"

@interface RWBuildController : NSObject <NSApplicationDelegate, RWBuildWorkerDelegate>

@property (strong) id<RWBuildWorker> worker;
@property (strong) id<RWDeviceConnectionFactory> connectionFactory;

- (void)notifyFileChanged:(NSString *)path;
- (void)tickConnection;

@end
