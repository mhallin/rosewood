#include "rosewood/utils/url_downloader.h"

#include "rosewood/graphics/texture.h"
#include "rosewood/graphics/image_loader.h"

#if TARGET_OS_MAC && !TARGET_OS_IPHONE
#include "rosewood/osx/RWGLView.h"
#endif

using rosewood::graphics::Texture;
using rosewood::graphics::image_loader::async_load_texture;

typedef void (^RWDownloaderCompletionHandler)(NSData *data, BOOL error);

@interface RWDownloader : NSObject

- (void)fetchURL:(NSString *)url completionHandler:(RWDownloaderCompletionHandler)completionHandler;
- (void)fetchURL:(NSString *)url cacheKey:(NSString *)fileName completionHandler:(RWDownloaderCompletionHandler)completionHandler;

@end

@interface RWDownloader () <NSURLConnectionDelegate, NSURLConnectionDataDelegate>
@end

@implementation RWDownloader {
    RWDownloaderCompletionHandler _completionHandler;
    NSString *_filePath;
    NSMutableData *_receivedData;
}

- (void)fetchURL:(NSString *)url completionHandler:(RWDownloaderCompletionHandler)completionHandler {
    _completionHandler = completionHandler;

    [self startFetchURL:[NSURL URLWithString:url]];
}

- (void)fetchURL:(NSString *)url cacheKey:(NSString *)fileName completionHandler:(RWDownloaderCompletionHandler)completionHandler {
    NSString *cacheDir = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES)[0];
    NSString *rwCacheDir = [cacheDir stringByAppendingPathComponent:@"rosewood"];
    NSString *filePath = [rwCacheDir stringByAppendingPathComponent:fileName];

    if (![[NSFileManager defaultManager] fileExistsAtPath:rwCacheDir]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:rwCacheDir
                                  withIntermediateDirectories:YES
                                                   attributes:nil
                                                        error:nil];
    }

    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        completionHandler([[NSData alloc] initWithContentsOfFile:filePath], NO);
    }
    else {
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    }

    _completionHandler = completionHandler;
    _filePath = filePath;

    [self startFetchURL:[NSURL URLWithString:url]];
}

- (void)startFetchURL:(NSURL *)url {
    _receivedData = [NSMutableData data];

    NSURLRequest *req = [NSURLRequest requestWithURL:url
                                         cachePolicy:NSURLRequestReloadIgnoringCacheData
                                     timeoutInterval:30];
    NSURLConnection *c = [[NSURLConnection alloc] initWithRequest:req delegate:self];
    [c start];
}

- (void)connection:(NSURLConnection *)__unused connection didReceiveData:(NSData *)data {
    [_receivedData appendData:data];
}

- (void)connection:(NSURLConnection *)__unused connection didFailWithError:(NSError *)error {
    NSLog(@"Download of %@ failed: %@", _filePath, error);
    _completionHandler(nil, YES);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)__unused connection {
    if (_filePath) {
        [_receivedData writeToFile:_filePath atomically:YES];
    }
    _completionHandler(_receivedData, NO);
}

@end

namespace rosewood { namespace utils {

    void fetch_url(std::string url, download_completion_handler completion_handler) {
        [[RWDownloader alloc] fetchURL:@(url.c_str()) completionHandler:^(NSData *data, BOOL error) {
            completion_handler(std::string((const char *)data.bytes, data.length), !!error);
        }];
    }

    void fetch_cached_url(std::string url, std::string cache_key,
                          download_completion_handler completion_handler) {
        [[RWDownloader alloc] fetchURL:@(url.c_str()) cacheKey:@(cache_key.c_str()) completionHandler:^(NSData *data, BOOL error) {
            completion_handler(std::string((const char *)data.bytes, data.length), !!error);
        }];
    }

    void fetch_remote_texture(std::string url, std::string filename,
                              std::function<void(std::shared_ptr<graphics::Texture>)> callback) {
        fetch_cached_url(url, filename, [&](std::string data, BOOL error){
            if (error) {
                callback(nullptr);
            }
            else {
                async_load_texture(data, callback);
            }
        });
    }

//    void fetch_remote_world_ids(std::function<void(std::vector<std::string>)> callback) {
//        [[RWDownloader alloc] fetchURL:@"http://blocksworld.com/api/v1/worlds" localFilename:@"worlds.json" useCache:NO callback:^(NSData *data) {
//            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
//                id object = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
//                std::vector<std::string> ids;
//                for (NSDictionary *world in object[@"worlds"]) {
//                    ids.push_back([world[@"id"] description].UTF8String);
//                }
//                
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    callback(ids);
//                });
//            });
//         }];
//    }

#if TARGET_OS_MAC && !TARGET_OS_IPHONE

    
#endif
    
} }
