#ifndef __ROSEWOOD_CORE_RESOURCE_MANAGER_H__
#define __ROSEWOOD_CORE_RESOURCE_MANAGER_H__

#include <memory>
#include <string>
#include <functional>
#include <vector>

#include <sys/stat.h>

namespace rosewood { namespace core {
    
    class AssetView;
    
    class Asset {
    public:
        Asset(const std::string &file_contents);

        const std::string &str() const;
        
        void set_file_contents(const std::string &file_contents);
        
        Asset(const Asset&) = delete;
        Asset &operator=(const Asset&) = delete;
        
    private:
        std::vector<std::weak_ptr<AssetView>> _views;
        std::string _file_contents;

        friend std::shared_ptr<AssetView> create_view(const std::shared_ptr<Asset> &asset,
                                                      const std::function<void()> &change_callback);
    };
    
    class AssetView {
    public:
        AssetView(const std::shared_ptr<Asset> &asset,
                  const std::function<void()> &change_callback);
        
        const std::string &str() const;
        
        AssetView(const AssetView&) = delete;
        AssetView &operator=(const AssetView&) = delete;
        
    private:
        std::shared_ptr<Asset> _asset;
        std::function<void()> _change_callback;

        void notify_change() const;
        
        friend class Asset;
        friend std::shared_ptr<AssetView> create_view(const std::shared_ptr<Asset> &asset,
                                                      const std::function<void()> &change_callback);
    };
    
    struct FileInfo {
        bool exists;
        struct timespec mtime;
    };
    
    class IResourceLoader {
    public:
        virtual ~IResourceLoader() = 0;
        
        virtual FileInfo find_file(std::string path) const = 0;
        virtual std::string read_file(std::string path) const = 0;
    };
    
    void add_resource_loader(std::unique_ptr<IResourceLoader> loader);
    
    std::shared_ptr<Asset> get_resource(std::string path);
    std::shared_ptr<AssetView> create_view(const std::shared_ptr<Asset> &asset,
                                           const std::function<void()> &change_callback);

    
    void notify_file_changed(std::string path);
    
} }

#endif
