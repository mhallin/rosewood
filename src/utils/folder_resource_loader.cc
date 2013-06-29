#include "rosewood/utils/folder_resource_loader.h"

#include <sys/stat.h>

#include <fstream>
#include <iterator>
#include <iostream>

using rosewood::core::IResourceLoader;

using rosewood::utils::FolderResourceLoader;

FolderResourceLoader::FolderResourceLoader(const std::string &root) : _root(root) { }
FolderResourceLoader::~FolderResourceLoader() { }

rosewood::core::FileInfo FolderResourceLoader::find_file(std::string path) const {
	path = _root + "/asset-build/" + path;

	rosewood::core::FileInfo i;
	struct stat s;

	if (stat(path.c_str(), &s)) {
		std::cerr << "File " << path << " does not exist\n";
		i.exists = false;
	}
	else {
		i.exists = true;
		i.mtime = s.st_mtimespec;
	}

	return i;
}

std::string FolderResourceLoader::read_file(std::string path) const {
	path = _root + "/asset-build/" + path;

	std::ifstream stream(path);
	if (stream.bad()) {
		std::cerr << "File " << path << " does not exist\n";
	}

	return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}
