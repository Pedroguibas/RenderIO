#pragma once

#include <string>
#include <glad/gl.h>
using std::string;

class Texture {
  private:
    GLuint id;
    int width;
    int height;

  public:
    Texture(const string &path);
    ~Texture();

    void bind(GLuint unit) const;
    void unbind() const;
    void setParam(GLenum name, GLint param);
    void setDefaultParams();
};