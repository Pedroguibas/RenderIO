#include "Texture.h"

#include <stb_image.h>
#include <stdexcept>
#include <string>

Texture::Texture(const std::filesystem::path &path) {
  int channels;

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data =
    stbi_load(path.string().c_str(), &width, &height, &channels, 0);

  if (!data)
    throw std::invalid_argument(
      std::string("Unable to locate texture:\n") + path.string()
    );

  GLenum format = formatFromChannels(channels);

  create(data, format, format);

  stbi_image_free(data);
}
Texture::Texture(
  const void *data, int width, int height, GLenum format, GLenum internalFormat
)
: width(width),
  height(height) {

  create(data, format, internalFormat);
}
Texture::Texture(const unsigned char *data, int width, int height, int channels)
: width(width),
  height(height) {

  GLenum format = formatFromChannels(channels);

  create(data, format, format);
}
Texture::~Texture() {
  glDeleteTextures(1, &id);
}

void Texture::setParam(GLenum name, GLint param) {
  glTexParameteri(GL_TEXTURE_2D, name, param);
}
void Texture::setDefaultParams() {
  setParam(GL_TEXTURE_WRAP_S, GL_REPEAT);
  setParam(GL_TEXTURE_WRAP_T, GL_REPEAT);
  setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::unbind() const {
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::create(const void *data, GLenum format, GLenum internalFormat) {
  glGenTextures(1, &id);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, id);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glTexImage2D(
    GL_TEXTURE_2D,
    0,
    internalFormat,
    width,
    height,
    0,
    format,
    GL_UNSIGNED_BYTE,
    data
  );

  glGenerateMipmap(GL_TEXTURE_2D);
  setDefaultParams();
}

GLenum Texture::formatFromChannels(int channels) {
  switch (channels) {
  case 1:
    return GL_RED;

  case 3:
    return GL_RGB;

  case 4:
    return GL_RGBA;

  default:
    throw std::invalid_argument(std::string("Invalid Texture channel count"));
  }
}