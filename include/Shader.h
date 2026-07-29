#pragma once

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
using std::string;

class Shader {
  private:
    static std::unordered_map<string, std::unique_ptr<Shader>> cache;
    static string makeKey(
      const string &vert, const string &frag, const std::vector<string> &defines
    );
    GLuint id;
    std::unordered_map<string, GLint> m_locations;
    static GLuint compileShader(const GLuint type, const char *src);
    static GLuint linkProgram(const GLuint vert, const GLuint frag);
    string readFile(const string &path);
    GLint getLocation(const string &name);
    string injectDefines(const string &src, const std::vector<string> &defines);

    Shader(
      const string &vert, const string &frag, const std::vector<string> &defines
    );

  public:
    ~Shader();

    static Shader *create(
      const string &vert,
      const string &frag,
      const std::vector<string> &defines = {}
    );

    void use();

    void setFloat(const string &name, float val);
    void setInt(const string &name, int val);
    void setVec2(const string &name, float x, float y);
    void setVec3(const string &name, float x, float y, float z);
    void setVec4(const string &name, float x, float y, float z, float w);
    void setVec2(const string &name, glm::vec2 vec);
    void setVec3(const string &name, glm::vec3 vec);
    void setVec4(const string &name, glm::vec4 vec);
    void setMat4(const string &name, glm::mat4 mat);
};