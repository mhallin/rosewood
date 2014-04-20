#ifndef __ROSEWOOD_UTILS_FOLDER_RESOURCE_LOADER_H__
#define __ROSEWOOD_UTILS_FOLDER_RESOURCE_LOADER_H__

#include <string>

#include "rosewood/core/resource_manager.h"

namespace rosewood { namespace utils {

    class FolderResourceLoader : public core::IResourceLoader {
    public:
        FolderResourceLoader(const std::string &root);
        virtual ~FolderResourceLoader() override;
        virtual core::FileInfo find_file(std::string path) const override;
        virtual std::string read_file(std::string path) const override;

    private:
        std::string _root;
    };

} }

#endif
