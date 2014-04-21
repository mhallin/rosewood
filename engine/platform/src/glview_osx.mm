//
//  RWGLView.m
//  Rosewood
//
//  Created by Magnus Hallin on 2012-11-23.
//  Copyright (c) 2012 Magnus Hallin. All rights reserved.
//

#import "rosewood/platform/glview_osx.h"

#include <mutex>

#include "rosewood/core/resource_manager.h"
#include "rosewood/core/memory.h"

#include "rosewood/utils/animatable.h"
#include "rosewood/utils/time.h"
#include "rosewood/utils/bundle_resource_loader_ns.h"

using rosewood::core::add_resource_loader;

using rosewood::utils::NSBundleResourceLoader;

static __weak RWGLView *gDefaultView = nil;

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

+ (RWGLView *)defaultGLView {
    return gDefaultView;
}

- (void)awakeFromNib {
    _needsReshape = 0;
    _isPaused = NO;

    self.pixelFormat = [[self class] defaultPixelFormat];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(frameDidChange:)
                                                 name:NSViewFrameDidChangeNotification
                                               object:self];

    add_resource_loader(make_unique<NSBundleResourceLoader>([NSBundle mainBundle]));

    [self _setupOpenGL];
    [self _startDisplayLink];

    gDefaultView = self;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];

    [_delegate rosewoodWillTerminate:self];

    CVDisplayLinkRelease(_displayLink);
}

- (void)setIsPaused:(BOOL)isPaused {
    if (isPaused == _isPaused) return;

    _isPaused = isPaused;

    if (_isPaused) {
        CVDisplayLinkStop(_displayLink);
    }
    else {
        CVDisplayLinkStart(_displayLink);
    }
}

- (void)_setupOpenGL {
    [self.openGLContext makeCurrentContext];
    rosewood::utils::mark_frame_beginning();

    [_delegate rosewoodDidInitialize:self];
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

- (void)updateScene {
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

    if (_needsReshape) {
        [_delegate rosewoodDidReshapeViewport:self];
        --_needsReshape;
    }

    [self updateScene];

    [_delegate rosewoodDrawDidTick:self];

    [self.openGLContext flushBuffer];

    return kCVReturnSuccess;
}

@end
