//
//  RWGLView.h
//  Rosewood
//
//  Created by Magnus Hallin on 2012-11-23.
//  Copyright (c) 2012 Magnus Hallin. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

@class RWGLView;

@protocol RWGLViewDelegate <NSObject>

- (void)rosewoodUpdateDidTick:(RWGLView *)view;
- (void)rosewoodDrawDidTick:(RWGLView *)view;
- (void)rosewoodDidInitialize:(RWGLView *)view;
- (void)rosewoodWillTerminate:(RWGLView *)view;
- (void)rosewoodDidReshapeViewport:(RWGLView *)view;

@end

@interface RWGLView : NSOpenGLView
@property (weak, nonatomic) IBOutlet id<RWGLViewDelegate> delegate;

@property (nonatomic) BOOL isPaused;

- (CVReturn)drawForTime:(const CVTimeStamp *)time;

@end
