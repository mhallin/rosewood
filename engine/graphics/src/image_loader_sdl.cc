#include "rosewood/graphics/image_loader.h"

#include <SDL/SDL_image.h>

#include "rosewood/core/logging.h"

#include "rosewood/graphics/texture.h"

namespace rosewood { namespace graphics { namespace image_loader {

    ImageData load_bytes(const std::string &file_contents) {
        const void *data_ptr = static_cast<const void*>(file_contents.c_str());
        SDL_RWops *rw = SDL_RWFromMem(const_cast<void*>(data_ptr), file_contents.size());
        SDL_Surface *surf = IMG_Load_RW(rw, 1);

        ImageData data;
        data.width = surf->w;
        data.height = surf->h;
        data.bytes.reserve(surf->w * surf->h * surf->format->BytesPerPixel);

        for (size_t i = 0; i < surf->w * surf->h * surf->format->BytesPerPixel; ++i) {
            data.bytes.emplace_back(static_cast<char*>(surf->pixels)[i]);
        }
        
        return data;
    }

} } }
