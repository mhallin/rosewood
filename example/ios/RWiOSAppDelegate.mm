//
//  RWiOSAppDelegate.m
//  RWSample
//
//  Created by Magnus Hallin on 2012-12-22.
//
//

#import "RWiOSAppDelegate.h"

#import "rosewood/ios/RWViewController.h"

#import "../sample_app.h"

typedef sample_app::RosewoodApp AppClass;

@interface RWiOSAppDelegate () <RWViewControllerDelegate> {
    AppClass *_app;
}
@end

@implementation RWiOSAppDelegate

- (BOOL)application:(UIApplication *)__unused application didFinishLaunchingWithOptions:(NSDictionary *)__unused launchOptions {
    RWViewController *vc = [[RWViewController alloc] init];
    vc.rwDelegate = self;

    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.window.rootViewController = vc;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)rosewoodDidInitialize:(RWViewController *)__unused vc {
    _app = AppClass::create();
}

- (void)rosewoodDidReshapeViewport:(RWViewController *)vc {
    _app->reshape_viewport((float)CGRectGetWidth(vc.view.frame),
                           (float)CGRectGetHeight(vc.view.frame));
}

- (void)rosewoodDrawDidTick:(RWViewController *)__unused vc {
    _app->draw();
}

- (void)rosewoodUpdateDidTick:(RWViewController *)__unused vc {
    _app->update();
}

- (void)rosewoodWillTerminate:(RWViewController *)__unused vc {
    AppClass::destroy(_app);
}

@end
