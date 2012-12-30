#ifndef __ROSEWOOD_GRAPHICS_TEXTURE_H__
#define __ROSEWOOD_GRAPHICS_TEXTURE_H__

#include <vector>

#include "rosewood/graphics/platform_gl.h"

namespace rosewood { namespace core {
    class Asset;
    class AssetView;
} }

namespace rosewood { namespace graphics {

    struct ImageData {
        std::vector<unsigned char> bytes;
        int width;
        int height;
    };
    
    class Texture {
    public:
        Texture(const std::shared_ptr<core::Asset> &image_asset);
        Texture(const ImageData &image_data);
        ~Texture();
        
        void bind() const;
        
    private:
        GLuint _texture;
        std::shared_ptr<core::AssetView> _image_asset;
        ImageData _image_data;
        
        void reload_image_asset();
        void reload_image_data();
    };

} }

#endif
