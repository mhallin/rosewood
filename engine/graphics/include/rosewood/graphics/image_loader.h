#ifndef __ROSEWOOD_GRAPHICS_IMAGE_LOADER_H__
#define __ROSEWOOD_GRAPHICS_IMAGE_LOADER_H__

#include <functional>
#include <vector>
#include <string>

namespace rosewood { namespace graphics {
    
    struct ImageData;

    class Texture;

    namespace image_loader {
        typedef std::function<void(std::shared_ptr<graphics::Texture>)> image_loader_completion_handler;

        graphics::ImageData load_bytes(const std::string &file_contents);
    }
    
} }

#endif
