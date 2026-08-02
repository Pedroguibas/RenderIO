#include "Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

std::unordered_map<string, std::unique_ptr<Shader>> Shader::cache;

Shader::Shader(
  const string &vert, const string &frag, const std::vector<string> &defines
) {

  string vertSrc = Shader::readFile(vert);
  string fragSrc = Shader::readFile(frag);

  vertSrc = injectDefines(vertSrc, defines);
  fragSrc = injectDefines(fragSrc, defines);

  GLuint vertShader = Shader::compileShader(GL_VERTEX_SHADER, vertSrc.c_str());
  GLuint fragShader =
    Shader::compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());

  id = Shader::linkProgram(vertShader, fragShader);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
}

Shader::~Shader() {
  glDeleteProgram(id);
}

string Shader::readFile(const string &path) const {
  std::ifstream file(path);

  if (!file.is_open())
    throw std::runtime_error("Failed to open shader:" + path);

  std::stringstream buffer;

  buffer << file.rdbuf();

  return buffer.str();
}

GLuint Shader::compileShader(const GLuint type, const char *src) {
  GLuint shader = glCreateShader(type);

  glShaderSource(shader, 1, &src, nullptr);

  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    char infoLog[512];

    glGetShaderInfoLog(shader, 512, nullptr, infoLog);

    if (type == GL_VERTEX_SHADER)
      throw std::runtime_error(
        string("Vertex shader compilation failed:\n") + infoLog
      );

    else
      throw std::runtime_error(
        string("Fragment shader compilation failed:\n") + infoLog
      );
  }

  return shader;
}
GLuint Shader::linkProgram(const GLuint vert, const GLuint frag) {
  GLuint program = glCreateProgram();

  glAttachShader(program, vert);
  glAttachShader(program, frag);

  glLinkProgram(program);

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    char infoLog[512];

    glGetProgramInfoLog(program, 512, nullptr, infoLog);

    glDeleteProgram(program);

    throw std::runtime_error(
      string("Shader program linking failed:\n") + infoLog
    );
  }

  return program;
}

void Shader::use() const noexcept {
  glUseProgram(id);
}

GLint Shader::getLocation(const string &name) noexcept {
  auto it = m_locations.find(name);

  if (it != m_locations.end()) {
    return it->second;
  }

  GLint location = glGetUniformLocation(id, name.c_str());

  m_locations[name] = location;

  return location;
}

void Shader::setFloat(const string &name, float val) noexcept {
  glUniform1f(getLocation(name), val);
}
void Shader::setInt(const string &name, int val) noexcept {
  glUniform1i(getLocation(name), val);
}
void Shader::setBool(const string &name, bool val) noexcept {
  glUniform1i(getLocation(name), val);
}
void Shader::setVec2(const string &name, float x, float y) noexcept {
  glUniform2f(getLocation(name), x, y);
}
void Shader::setVec3(const string &name, float x, float y, float z) noexcept {
  glUniform3f(getLocation(name), x, y, z);
}
void Shader::setVec4(
  const string &name, float x, float y, float z, float w
) noexcept {
  glUniform4f(getLocation(name), x, y, z, w);
}
void Shader::setVec2(const string &name, glm::vec2 vec) noexcept {
  glUniform2f(getLocation(name), vec.x, vec.y);
}
void Shader::setVec3(const string &name, glm::vec3 vec) noexcept {
  glUniform3f(getLocation(name), vec.x, vec.y, vec.z);
}
void Shader::setVec4(const string &name, glm::vec4 vec) noexcept {
  glUniform4f(getLocation(name), vec.x, vec.y, vec.z, vec.w);
}
void Shader::setMat4(const string &name, glm::mat4 mat) noexcept {
  glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

string Shader::injectDefines(
  const string &src, const std::vector<string> &defines
) const noexcept {
  size_t versionEnd = src.find('\n');
  string result = src.substr(0, versionEnd + 1);

  for (const auto &def : defines) {
    result += "#define " + def + '\n';
  }

  result += src.substr(versionEnd + 1);
  return result;
}

string Shader::makeKey(
  const string &vert, const string &frag, const std::vector<string> &defines
) noexcept {
  string key = vert + '|' + frag;
  auto sorted = defines;
  std::sort(sorted.begin(), sorted.end());

  for (const auto &d : sorted)
    key += '|' + d;

  return key;
}

Shader *Shader::create(
  const string &vert, const string &frag, const std::vector<string> &defines
) {
  string key = makeKey(vert, frag, defines);
  auto it = Shader::cache.find(key);

  if (it != Shader::cache.end())
    return it->second.get();

  std::unique_ptr<Shader> shader(new Shader(vert, frag, defines));
  Shader *ptr = shader.get();
  Shader::cache[key] = std::move(shader);
  return ptr;
}
