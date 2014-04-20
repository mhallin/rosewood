//
//  RWViewController.h
//  Rosewood
//
//  Created by Magnus Hallin on 2012-12-23.
//
//

#import <GLKit/GLKit.h>

@class RWViewController;

@protocol RWViewControllerDelegate <NSObject>

- (void)rosewoodUpdateDidTick:(RWViewController *)view;
- (void)rosewoodDrawDidTick:(RWViewController *)view;
- (void)rosewoodDidInitialize:(RWViewController *)view;
- (void)rosewoodWillTerminate:(RWViewController *)view;
- (void)rosewoodDidReshapeViewport:(RWViewController *)view;

@end


@interface RWViewController : GLKViewController
@property (strong, nonatomic) EAGLContext *context;
@property (readonly) GLKView *glkView;

@property (weak, nonatomic) IBOutlet id<RWViewControllerDelegate> rwDelegate;

@end
