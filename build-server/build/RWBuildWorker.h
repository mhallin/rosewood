#import <Foundation/Foundation.h>

@protocol RWBuildWorker;

@protocol RWBuildWorkerDelegate <NSObject>
@required

- (void)worker:(id<RWBuildWorker>)worker
reportsBuildSuccessFromSources:(NSArray *)sourcePaths
toDestinations:(NSArray *)destinationPaths;

- (void)worker:(id<RWBuildWorker>)worker
reportsBuildFailureFromSources:(NSArray *)sourcePaths
toDestinations:(NSArray *)destinationPaths;

- (void)worker:(id<RWBuildWorker>)worker
needsToExecuteOperation:(NSOperation *)operation;

@end

@protocol RWBuildWorker <NSObject>
@required

@property id<RWBuildWorkerDelegate> delegate;
@property NSString *projectRoot;

- (NSOperation *)constructBuildGraphOperation;
- (NSOperation *)rebuildFileOperation:(NSString *)path;

@end
