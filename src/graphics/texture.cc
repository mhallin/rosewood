#include "rosewood/graphics/texture.h"

#include "rosewood/core/resource_manager.h"

#include "rosewood/graphics/gl_state.h"
#include "rosewood/graphics/gl_func.h"
#include "rosewood/graphics/image_loader.h"

using rosewood::core::Asset;

using rosewood::graphics::Texture;

std::shared_ptr<Texture> Texture::create(const std::string &resource_path) {
    return std::make_shared<Texture>(core::get_resource(resource_path));
}

Texture::Texture(const std::shared_ptr<Asset> &image_asset)
: _image_asset(core::create_view(image_asset, [&] { reload_image_asset(); })) {
    glGenTextures(1, &_texture);
    bind();
    
    reload_image_asset();
}

Texture::Texture(const ImageData &image_data)
: _image_data(image_data) {
    glGenTextures(1, &_texture);
    
    reload_image_data();
}

Texture::~Texture() {
    gl_state::delete_texture(_texture);
}

void Texture::bind() const {
    gl_state::bind_texture(_texture);
}

void Texture::reload_image_asset() {
    _image_data = image_loader::load_bytes(_image_asset->str());
    reload_image_data();
}

void Texture::reload_image_data() {
    bind();
    
    GL_FUNC(glTexImage2D)(GL_TEXTURE_2D, 0, GL_RGBA,
                          _image_data.width, _image_data.height, 0,
                          GL_RGBA, GL_UNSIGNED_BYTE, &_image_data.bytes[0]);
    GL_FUNC(glGenerateMipmap)(GL_TEXTURE_2D);
    GL_FUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL_FUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GL_FUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GL_FUNC(glTexParameteri)(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
