//
//  RWGLView.m
//  Rosewood
//
//  Created by Magnus Hallin on 2012-11-23.
//  Copyright (c) 2012 Magnus Hallin. All rights reserved.
//

#import "rosewood/osx/RWGLView.h"

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/memory.h"

#include "rosewood/utils/animatable.h"
#include "rosewood/utils/time.h"
#include "rosewood/ns/bundle_resource_loader.h"

using rosewood::core::add_resource_loader;

using rosewood::ns::NSBundleResourceLoader;

static CVReturn display_link_callback(__unused CVDisplayLinkRef displayLink,
                                      __unused const CVTimeStamp *inNow,
                                      const CVTimeStamp *inOutputTime,
                                      __unused CVOptionFlags flagsIn,
                                      __unused CVOptionFlags *flagsOut,
                                      void *displayLinkContext) {
    @autoreleasepool {
        RWGLView *view = (__bridge RWGLView*)displayLinkContext;
        return [view drawForTime:inOutputTime];
    }
}

@implementation RWGLView {
    BOOL _needsReshape;
    BOOL _haveViewport;

    CVDisplayLinkRef _displayLink;

    NSTimeInterval _lastFrame;

    NSTimer *_updateTimer;
}

+ (NSOpenGLPixelFormat *)defaultPixelFormat {
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFASamples, 4,
        NSOpenGLPFAMultisample,
        NSOpenGLPFASampleBuffers, 2,
        0
    };

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
}

- (void)awakeFromNib {
    _needsReshape = NO;
    _haveViewport = NO;
    _isPaused = NO;

    self.pixelFormat = [[self class] defaultPixelFormat];

    add_resource_loader(make_unique<NSBundleResourceLoader>([NSBundle mainBundle]));

    [self _setupOpenGL];
    [self _startTimer];
    [self _startDisplayLink];
}

- (void)dealloc {
    [_delegate rosewoodWillTerminate:self];

    CVDisplayLinkRelease(_displayLink);
}

- (void)setIsPaused:(BOOL)isPaused {
    if (isPaused == _isPaused) return;

    _isPaused = isPaused;

    if (_isPaused) {
        CVDisplayLinkStop(_displayLink);
        [_updateTimer invalidate];
        _updateTimer = nil;
    }
    else {
        CVDisplayLinkStart(_displayLink);
        [self _startTimer];
    }
}

- (void)_setupOpenGL {
    [self.openGLContext makeCurrentContext];
    rosewood::utils::mark_frame_beginning();

    [_delegate rosewoodDidInitialize:self];
}

- (void)_startTimer {
    _updateTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/240.0
                                                    target:self
                                                  selector:@selector(updateScene:)
                                                  userInfo:nil
                                                   repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:_updateTimer forMode:NSEventTrackingRunLoopMode];
}

- (void)_startDisplayLink {
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);

    CVDisplayLinkSetOutputCallback(_displayLink, &display_link_callback, (__bridge void*)self);

    CGLContextObj cglCtx = (CGLContextObj)self.openGLContext.CGLContextObj;
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)self.pixelFormat.CGLPixelFormatObj;
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglCtx, cglPixelFormat);

    CVDisplayLinkStart(_displayLink);
}

- (void)prepareOpenGL {
    GLint swap = 1;
    [self.openGLContext setValues:&swap forParameter:NSOpenGLCPSwapInterval];
    [super prepareOpenGL];
}

- (void)setFrame:(NSRect)frameRect {
    _needsReshape = YES;
    [super setFrame:frameRect];
}

- (void)updateScene:(id)__unused sender {
    NSTimeInterval now = [NSDate timeIntervalSinceReferenceDate];
    if (now - _lastFrame >= 1.0/60.0) {
        rosewood::utils::mark_frame_beginning();
        rosewood::utils::tick_all_animations();
        [_delegate rosewoodUpdateDidTick:self];
        _lastFrame = now;
    }
}

- (CVReturn)drawForTime:(const CVTimeStamp *)__unused time {
    [self.openGLContext makeCurrentContext];

    if (_needsReshape || !_haveViewport) {
        [_delegate rosewoodDidReshapeViewport:self];
        _needsReshape = NO;
        _haveViewport = YES;
    }

    [_delegate rosewoodDrawDidTick:self];

    [self.openGLContext flushBuffer];

    return kCVReturnSuccess;
}

@end
