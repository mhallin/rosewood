#import <UIKit/UIKit.h>

#include "rosewood/graphics/image_loader.h"

#include "rosewood/graphics/texture.h"

namespace rosewood { namespace graphics { namespace image_loader {

    ImageData load_bytes(const std::string &file_contents) {
        const int kBitsPerComponent = 8;
        const int kNumComponents = 4;
        const int kBytesPerPixel = kBitsPerComponent * kNumComponents / 8;
        
        NSData *data = [NSData dataWithBytes:file_contents.c_str()
                                      length:file_contents.size()];
        UIImage *image = [UIImage imageWithData:data];
        
        int width = (int)image.size.width;
        int height = (int)image.size.height;
        std::vector<unsigned char> bytes(width * height * kNumComponents);
        
        
        
        CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
        CGContextRef context = CGBitmapContextCreate(&bytes[0], width, height,
                                                     kBitsPerComponent,
                                                     kBytesPerPixel * width,
                                                     color_space,
                                                     kCGImageAlphaPremultipliedLast);
        CGContextClearRect(context, CGRectMake(0, 0, width, height));
        CGContextDrawImage(context, CGRectMake(0, 0, width, height), image.CGImage);
        CGContextRelease(context);
        CGColorSpaceRelease(color_space);
        
        ImageData idata;
        idata.width = width;
        idata.height = height;
        idata.bytes = bytes;
        
        return idata;
    }

    void async_load_texture(const std::string &file_contents,
                            image_loader_completion_handler completion_handler) {
        static NSOperationQueue *q = nil;
        static EAGLContext *ctx = nil;

        if (!q) {
            q = [[NSOperationQueue alloc] init];
            q.maxConcurrentOperationCount = 1;
            q.name = @"Background Texture Loader";

            EAGLContext *current_context = [EAGLContext currentContext];
            ctx = [[EAGLContext alloc] initWithAPI:current_context.API
                                        sharegroup:current_context.sharegroup];
        }

        __block image_loader_completion_handler ch = completion_handler;

        [q addOperationWithBlock:^{
            [EAGLContext setCurrentContext:ctx];
            auto tex = std::make_shared<Texture>(load_bytes(file_contents));
            [EAGLContext setCurrentContext:nil];

            dispatch_async(dispatch_get_main_queue(), ^{
                ch(tex);
            });
        }];
    }

} } }
