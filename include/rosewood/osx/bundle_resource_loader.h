#ifndef __WORLDWIND_BUNDLE_RESOURCE_LOADER_H__
#define __WORLDWIND_BUNDLE_RESOURCE_LOADER_H__

#include "rosewood/core/resource_manager.h"

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#else
typedef NSBundle void
#endif

namespace rosewood { namespace utils {

    class NSBundleResourceLoader : public core::IResourceLoader {
    public:
        NSBundleResourceLoader(NSBundle *bundle);
        virtual ~NSBundleResourceLoader() override;
        virtual core::FileInfo find_file(std::string path) const override;
        virtual std::string read_file(std::string path) const override;
        
    private:
        NSBundle *_bundle;
    };
    
} }

#endif
