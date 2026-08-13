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
    ~Texture();

    void bind(GLuint unit) const;
    void unbind() const;
    void setParam(GLenum name, GLint param);
    void setDefaultParams();
};