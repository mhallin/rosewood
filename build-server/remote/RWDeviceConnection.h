//
//  RWDeviceConnection.h
//  Rosewood
//
//  Created by Magnus Hallin on 2012-12-29.
//
//

#import <Foundation/Foundation.h>

@protocol RWDeviceConnection <NSObject>
@required

@property (readonly) NSNumber *queueSize;

- (void)tickLoop;
- (void)disconnect;
- (void)appendCommand:(NSArray *)command;

@end

@protocol RWDeviceConnectionFactory <NSObject>
@required

- (id<RWDeviceConnection>)createConnectionToHost:(NSString *)host port:(NSNumber *)port;

@end
