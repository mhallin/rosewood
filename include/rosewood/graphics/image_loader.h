#ifndef __ROSEWOOD_ENGINE_IMAGE_LOADER_H__
#define __ROSEWOOD_ENGINE_IMAGE_LOADER_H__

#include <functional>
#include <vector>
#include <string>

namespace rosewood { namespace graphics {
    
    struct ImageData;

    class Texture;
    
    namespace image_loader {
        typedef std::function<void(std::shared_ptr<Texture>)> image_loader_completion_handler;

        ImageData load_bytes(const std::string &file_contents);
        void async_load_texture(const std::string &file_contents,
                                image_loader_completion_handler completion_handler);
    }
    
} }

#endif
