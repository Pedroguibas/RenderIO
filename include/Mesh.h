#pragma once

#include <glad/gl.h>
#include <vector>
#include "Vertex.h"
#include "Shader.h"
using std::vector;

class Mesh {
  private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    unsigned int materialIndex;
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    GLsizei indiceCount;

    void addAttribute(
      GLuint index,
      GLuint size,
      GLboolean normalized,
      GLsizei stride,
      const void *pointer
    );

  public:
    Mesh(
      const vector<Vertex> &vertices,
      const vector<unsigned int> &indicies,
      unsigned int materialIndex = 0
    );
    ~Mesh();

    void draw() const;

    const vector<Vertex> &getVertices() const noexcept;

    unsigned int getMaterialIndex() const noexcept;

    void setMaterialIndex(unsigned int idx);

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) noexcept;
    Mesh &operator=(Mesh &&) noexcept;
};