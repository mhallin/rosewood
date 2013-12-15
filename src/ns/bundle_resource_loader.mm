#include "rosewood/ns/bundle_resource_loader.h"

using rosewood::core::IResourceLoader;

using rosewood::ns::NSBundleResourceLoader;

NSBundleResourceLoader::NSBundleResourceLoader(NSBundle *bundle) : _bundle(bundle) { }
NSBundleResourceLoader::~NSBundleResourceLoader() { }
    
rosewood::core::FileInfo NSBundleResourceLoader::find_file(std::string path) const {
    NSString *asset_path = [@"asset-build" stringByAppendingPathComponent:@(path.c_str())];
    NSString *path_str = [_bundle pathForResource:asset_path ofType:nil];
    rosewood::core::FileInfo i;
    
    if (path_str) {
        struct stat s;
        stat([path_str UTF8String], &s);
        i.exists = true;
        i.mtime = s.st_mtimespec;
    }
    else {
        i.exists = false;
    }
    return i;
}

std::string NSBundleResourceLoader::read_file(std::string path) const {
    NSString *asset_path = [@"asset-build" stringByAppendingPathComponent:@(path.c_str())];
    NSString *path_str = [_bundle pathForResource:asset_path ofType:nil];
    NSData *contents = [NSData dataWithContentsOfFile:path_str];
    return std::string((char*)contents.bytes, contents.length);
}
