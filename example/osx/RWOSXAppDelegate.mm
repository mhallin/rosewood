//
//  RWOSXAppDelegate.mm
//  RWSample
//
//  Created by Magnus Hallin on 2012-12-22.
//
//

#import "RWOSXAppDelegate.h"

#import "rosewood/osx/RWGLView.h"

#import "../sample_app.h"

typedef sample_app::RosewoodApp AppClass;

@interface RWOSXAppDelegate () <RWGLViewDelegate> {
    AppClass *_app;
}
@end

@implementation RWOSXAppDelegate

- (void)rosewoodDidInitialize:(RWGLView *)__unused view {
    _app = AppClass::create();
}

- (void)rosewoodDidReshapeViewport:(RWGLView *)view {
    _app->reshape_viewport((float)CGRectGetWidth(view.frame),
                           (float)CGRectGetHeight(view.frame));
}

- (void)rosewoodDrawDidTick:(RWGLView *)__unused view {
    _app->draw();
}

- (void)rosewoodUpdateDidTick:(RWGLView *)__unused view {
    _app->update();
}

- (void)rosewoodWillTerminate:(RWGLView *)__unused view {
    AppClass::destroy(_app);
}


@end
