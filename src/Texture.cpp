#include "Texture.h"

#include <stb_image.h>
#include <stdexcept>

Texture::Texture(const string &path) {
  int channels;

  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

  if (!data)
    throw std::invalid_argument(string("Unable to locate texture:\n") + path);

  GLenum format;
  switch (channels) {
  case 1:
    format = GL_RED;
    break;

  case 3:
    format = GL_RGB;
    break;

  case 4:
    format = GL_RGBA;
    break;

  default:
    throw std::invalid_argument(string("Unsuported texture format:\n" + path));
  }

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexImage2D(
    GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
  );

  setDefaultParams();
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);
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
