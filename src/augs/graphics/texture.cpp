#include "augs/graphics/OpenGL_includes.h"
#include "augs/graphics/texture.h"
#include "augs/graphics/pbo.h"

namespace augs {
	namespace graphics {
		void texture::bind() const {
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, id));
		}

		void texture::unbind() {
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		}

		texture::texture(const vec2u new_size, const rgba* const source) {
			create();
			texImage2D(new_size, std::addressof(source->r));
		}

		texture::texture(const image& source) {
			create();
			texImage2D(source.get_size(), source.get_data());
		}

		void texture::start_upload_from(const pbo& p) {
			p.set_as_current();
			GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, 0));
		}

		void texture::texImage2D(const vec2u new_size, const unsigned char* const source) {
			size = new_size;

			(void)source;
			GL_CHECK(glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				size.x,
				size.y,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				source
			));
		}

		void texture::create() {
			GL_CHECK(glGenTextures(1, &id));

			bind();

			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			built = true;
		}

		texture::~texture() {
			destroy();
		}

		texture::texture(texture&& b) :
			id(b.id),
			built(b.built),
			size(b.size)
		{
			b.built = false;
		}

		texture& texture::operator=(texture&& b) {
			destroy();

			id = b.id;
			built = b.built;
			size = b.size;

			b.built = false;

			return *this;
		}

		void texture::destroy() {
			if (built) {
				GL_CHECK(glDeleteTextures(1, &id));
				built = false;
			}
		}
	}
}