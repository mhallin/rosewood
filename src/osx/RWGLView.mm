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
#include "rosewood/utils/bundle_resource_loader.h"

using rosewood::core::add_resource_loader;

using rosewood::utils::NSBundleResourceLoader;

@implementation RWGLView {
    BOOL _needsReshape;
    BOOL _haveViewport;
}

+ (NSOpenGLPixelFormat *)defaultPixelFormat {
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADepthSize, 32,
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

    self.pixelFormat = [[self class] defaultPixelFormat];

    add_resource_loader(make_unique<NSBundleResourceLoader>([NSBundle mainBundle]));

    [self _setupOpenGL];
    [self _startTimer];
}

- (void)dealloc {
    [_delegate rosewoodWillTerminate:self];
}

- (void)_setupOpenGL {
    [self.openGLContext makeCurrentContext];
    rosewood::utils::mark_frame_beginning();

    [_delegate rosewoodDidInitialize:self];
}

- (void)_startTimer {
    [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                     target:self
                                   selector:@selector(updateScene:)
                                   userInfo:nil
                                    repeats:YES];
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
    rosewood::utils::mark_frame_beginning();
    rosewood::utils::tick_all_animations();
    [_delegate rosewoodUpdateDidTick:self];
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)__unused dirtyRect {
    [self.openGLContext makeCurrentContext];

    if (_needsReshape || !_haveViewport) {
        [_delegate rosewoodDidReshapeViewport:self];
        _needsReshape = NO;
        _haveViewport = YES;
    }

    [_delegate rosewoodDrawDidTick:self];
    [self.openGLContext flushBuffer];
}

@end
