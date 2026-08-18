#pragma once

#include <glad/gl.h>
#include <filesystem>

class Texture {
  private:
    GLuint id;
    int width;
    int height;

  public:
    Texture(const std::filesystem::path &path);
    Texture(const unsigned char *data, int width, int height, int channels);
    Texture(
      const void *data,
      int width,
      int height,
      GLenum format,
      GLenum internalFormat = GL_RGBA
    );
    ~Texture();

    void bind(GLuint unit) const;
    void unbind() const;
    void setParam(GLenum name, GLint param);
    void setDefaultParams();
    static GLenum formatFromChannels(int channels);

  private:
    void
    create(const void *data, GLenum format, GLenum internalFormat = GL_RGBA);
};