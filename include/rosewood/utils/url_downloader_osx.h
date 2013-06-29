#ifndef __WORLDWIND_URL_DOWNLOADER_H__
#define __WORLDWIND_URL_DOWNLOADER_H_

#include <functional>
#include <string>
#include <vector>

namespace rosewood { namespace graphics {
    class Texture;
} }

namespace rosewood { namespace utils {

    // download_completion_handler(result, error)
    typedef std::function<void(std::string, bool)> download_completion_handler;

    void fetch_url(std::string url, download_completion_handler completion_handler);
    void fetch_cached_url(std::string url, std::string cache_key,
                          download_completion_handler completion_handler);
    
    void fetch_remote_texture(std::string url, std::string filename,
                              std::function<void(std::shared_ptr<graphics::Texture>)> callback);

} }

#endif
