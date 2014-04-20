#include "rosewood/graphics/image_loader.h"

#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>

#include "rosewood/graphics/texture.h"

namespace rosewood { namespace graphics { namespace image_loader {

    ImageData load_bytes(const std::string &file_contents) {
        const int kBitsPerComponent = 8;
        const int kNumComponents = 4;
        const int kBytesPerPixel = kBitsPerComponent * kNumComponents / 8;
        
        NSData *data = [NSData dataWithBytes:file_contents.c_str()
                                      length:file_contents.size()];
        NSImage *image = [[NSImage alloc] initWithData:data];
        
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
        CGContextDrawImage(context, CGRectMake(0, 0, width, height), [image CGImageForProposedRect:NULL context:nil hints:nil]);
        CGContextRelease(context);
        CGColorSpaceRelease(color_space);
        
        ImageData idata;
        idata.width = width;
        idata.height = height;
        idata.bytes = bytes;
        
        return idata;
    }

} } }
