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
    ) noexcept;
    GLuint id;
    std::unordered_map<string, GLint> m_locations;
    static GLuint compileShader(const GLuint type, const char *src);
    static GLuint linkProgram(const GLuint vert, const GLuint frag);
    string readFile(const string &path) const;
    GLint getLocation(const string &name) noexcept;
    string injectDefines(
      const string &src, const std::vector<string> &defines
    ) const noexcept;

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

    void use() const noexcept;

    void setFloat(const string &name, float val) noexcept;
    void setInt(const string &name, int val) noexcept;
    void setBool(const string &name, bool val) noexcept;
    void setVec2(const string &name, float x, float y) noexcept;
    void setVec3(const string &name, float x, float y, float z) noexcept;
    void
    setVec4(const string &name, float x, float y, float z, float w) noexcept;
    void setVec2(const string &name, glm::vec2 vec) noexcept;
    void setVec3(const string &name, glm::vec3 vec) noexcept;
    void setVec4(const string &name, glm::vec4 vec) noexcept;
    void setMat4(const string &name, glm::mat4 mat) noexcept;
};