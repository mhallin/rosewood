#include "rosewood/graphics/image_loader.h"

#include <png.h>

#include <stdexcept>

#include "rosewood/graphics/texture.h"

namespace rosewood { namespace graphics { namespace image_loader {

	struct ReadBuffer {
		size_t position;
		const unsigned char *data;
	};

	static void read_png_buffer(png_structp png_ptr, png_bytep out_bytes, png_size_t bytes_to_read) {
		ReadBuffer *buffer = static_cast<ReadBuffer *>(png_get_io_ptr(png_ptr));
		memcpy(out_bytes, buffer->data + buffer->position, bytes_to_read);
		buffer->position += bytes_to_read;
	}

    ImageData load_bytes(const std::string &file_contents) {
		if (png_sig_cmp(reinterpret_cast<const unsigned char *>(file_contents.c_str()), 0, 8)) {
			throw std::runtime_error("Invalid png header");
		}

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!png_ptr) throw std::runtime_error("Could not create png read struct");

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) throw std::runtime_error("Could not create png info struct");

		ReadBuffer buffer{ 0, reinterpret_cast<const unsigned char*>(file_contents.c_str()) };

		png_set_read_fn(png_ptr, static_cast<void*>(&buffer), &read_png_buffer);

		png_read_info(png_ptr, info_ptr);

		int width = png_get_image_width(png_ptr, info_ptr);
		int height = png_get_image_height(png_ptr, info_ptr);

		png_read_update_info(png_ptr, info_ptr);

		size_t rowLength = png_get_rowbytes(png_ptr, info_ptr);
		std::vector<unsigned char> pixels(rowLength * height);

		unsigned char *rowPointers[height];
		for (int y = 0; y < height; ++y) {
			rowPointers[y] = &pixels[y * rowLength];
		}

		png_read_image(png_ptr, rowPointers);

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		ImageData idata;
        idata.width = width;
        idata.height = height;
        idata.bytes = pixels;

        return idata;
    }

} } }
