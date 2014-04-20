#include "rosewood/utils/folder_resource_loader.h"

#include <sys/stat.h>

#include <fstream>
#include <iterator>
#include <iostream>

#include "rosewood/core/logging.h"

using rosewood::core::IResourceLoader;

using rosewood::utils::FolderResourceLoader;

FolderResourceLoader::FolderResourceLoader(const std::string &root) : _root(root) { }
FolderResourceLoader::~FolderResourceLoader() { }

rosewood::core::FileInfo FolderResourceLoader::find_file(std::string path) const {
    path = _root + path;

    rosewood::core::FileInfo i;
    struct stat s;

    if (stat(path.c_str(), &s)) {
        LOG(WARNING) << "File does not exist: " << path;
        i.exists = false;
    }
    else {
        i.exists = true;
#ifndef EMSCRIPTEN
        i.mtime = s.st_mtimespec;
#else
        i.mtime.tv_sec = 10000;
#endif
    }

    return i;
}

std::string FolderResourceLoader::read_file(std::string path) const {
    path = _root + path;

    std::ifstream stream(path);
    if (stream.bad()) {
        LOG(ERROR) << "File does not exist: " << path;
    }

    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}
