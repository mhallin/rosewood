#include "rosewood/core/resource_manager.h"

#include "rosewood/core/logging.h"

#include <unordered_map>

using rosewood::core::Asset;
using rosewood::core::AssetView;
using rosewood::core::IResourceLoader;

static std::vector<std::unique_ptr<IResourceLoader>> gResourceLoaders;
static std::unordered_map<std::string, std::weak_ptr<Asset>> gLoadedAssets;

Asset::Asset(const std::string &file_contents) : _file_contents(file_contents) { }

const std::string &Asset::str() const { return _file_contents; }

void Asset::set_file_contents(const std::string &file_contents) {
    _file_contents = file_contents;

    for (auto weak_view : _views) {
        auto view = weak_view.lock();
        if (!view) continue;

        view->notify_change();
    }
}

std::shared_ptr<AssetView> rosewood::core::create_view(const std::shared_ptr<Asset> &asset,
                                                       const std::function<void ()> &change_callback) {
    if (!asset) {
        LOG(ERROR, "Can not create asset view with a nullptr asset");
    }

    auto view = std::make_shared<AssetView>(asset, change_callback);
    asset->_views.emplace_back(view);
    return view;
}

AssetView::AssetView(const std::shared_ptr<Asset> &asset,
                     const std::function<void()> &change_callback)
: _asset(asset), _change_callback(change_callback) { }

const std::string &AssetView::str() const { return _asset->str(); }

void AssetView::notify_change() const {
    _change_callback();
}

IResourceLoader::~IResourceLoader() { }

void rosewood::core::add_resource_loader(std::unique_ptr<IResourceLoader> loader) {
    gResourceLoaders.push_back(std::move(loader));
}

static bool load_newest_asset_contents(const std::string &path, std::string *out_contents) {
    IResourceLoader *newest_loader = nullptr;
    struct timespec newest_spec{0, 0};

    if (gResourceLoaders.empty()) {
        LOG(WARNING, "There are no resource loaders registered");
    }

    for (const auto &loader : gResourceLoaders) {
        auto info = loader->find_file(path);
        if (info.exists && newest_spec.tv_sec < info.mtime.tv_sec) {
            newest_loader = loader.get();
            newest_spec = info.mtime;
        }
    }

    if (newest_loader) {
        *out_contents = newest_loader->read_file(path);
        return true;
    }

    return false;
}

std::shared_ptr<Asset> rosewood::core::get_resource(std::string path) {
    if (gLoadedAssets.find(path) == gLoadedAssets.end() || gLoadedAssets[path].expired()) {
        std::string file_contents;

        if (load_newest_asset_contents(path, &file_contents)) {
            auto asset = std::make_shared<Asset>(file_contents);
            gLoadedAssets[path] = asset;
            return asset;
        }

        LOG(WARNING) << "Could not find asset named " << path;
        return nullptr;
    }

    return gLoadedAssets[path].lock();
}

void rosewood::core::notify_file_changed(std::string path) {
    if (gLoadedAssets.find(path) == gLoadedAssets.end()) return;

    auto asset = get_resource(path);

    std::string file_contents;
    if (load_newest_asset_contents(path, &file_contents)) {
        asset->set_file_contents(file_contents);
    }
}
