//
//  RWGLView.m
//  Rosewood
//
//  Created by Magnus Hallin on 2012-11-23.
//  Copyright (c) 2012 Magnus Hallin. All rights reserved.
//

#import "rosewood/osx/RWGLView.h"

#include <mutex>

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
    int _needsReshape;

    CVDisplayLinkRef _displayLink;

    NSTimeInterval _lastFrame;

    NSTimer *_updateTimer;

    std::mutex *_draw_mutex;
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
    _needsReshape = 0;
    _isPaused = NO;

    _draw_mutex = new std::mutex();

    self.pixelFormat = [[self class] defaultPixelFormat];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(frameDidChange:)
                                                 name:NSViewFrameDidChangeNotification
                                               object:self];

    add_resource_loader(make_unique<NSBundleResourceLoader>([NSBundle mainBundle]));

    [self _setupOpenGL];
    [self _startTimer];
    [self _startDisplayLink];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [_delegate rosewoodWillTerminate:self];

    delete _draw_mutex;
    _draw_mutex = nullptr;

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

- (void)frameDidChange:(NSNotification *)__unused notification {
    _needsReshape = 2;
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)__unused dirtyRect {
    if (_needsReshape) {
        [self drawForTime:nullptr];
    }
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
    std::lock_guard<std::mutex> lock(*_draw_mutex);

    [self.openGLContext makeCurrentContext];

    if (_needsReshape) {
        [_delegate rosewoodDidReshapeViewport:self];
        --_needsReshape;
    }

    [_delegate rosewoodDrawDidTick:self];

    [self.openGLContext flushBuffer];

    return kCVReturnSuccess;
}

@end
