//
//  RWViewController.m
//  Rosewood
//
//  Created by Magnus Hallin on 2012-12-23.
//
//

#import "rosewood/platform/glviewcontroller_ios.h"

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/memory.h"

#include "rosewood/utils/animatable.h"
#include "rosewood/utils/bundle_resource_loader_ns.h"
#include "rosewood/utils/time.h"

using rosewood::core::add_resource_loader;

using rosewood::utils::NSBundleResourceLoader;

@implementation RWViewController {
    bool _skipFrame;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    self.preferredFramesPerSecond = 60;

    add_resource_loader(make_unique<NSBundleResourceLoader>([NSBundle mainBundle]));

    self.glkView.context = self.context;
    self.glkView.drawableDepthFormat = GLKViewDrawableDepthFormat24;

    _skipFrame = true;

    [self setupOpenGL];
}

- (void)dealloc {
    [self teardownOpenGL];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (GLKView *)glkView {
    return (GLKView *)self.view;
}

#pragma mark - Rosewood App

- (void)setupOpenGL {
    [EAGLContext setCurrentContext:self.context];
    rosewood::utils::mark_frame_beginning();

    [_rwDelegate rosewoodDidInitialize:self];
}

- (void)teardownOpenGL {
    [EAGLContext setCurrentContext:self.context];

    [_rwDelegate rosewoodWillTerminate:self];
}

- (void)update {
    rosewood::utils::mark_frame_beginning();
    rosewood::utils::tick_all_animations();
    [_rwDelegate rosewoodDidReshapeViewport:self];
    [_rwDelegate rosewoodUpdateDidTick:self];
}

- (void)glkView:(GLKView *)__unused view drawInRect:(CGRect)__unused rect {
    if (!_skipFrame) {
        [_rwDelegate rosewoodDrawDidTick:self];
    }
    _skipFrame = false;
}


@end
