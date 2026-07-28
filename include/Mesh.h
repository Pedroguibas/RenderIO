#pragma once

#include <glad/gl.h>
#include <vector>
#include "Vertex.h"
using std::vector;

class Mesh {
  private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    void addAttribute(
      GLuint index,
      GLuint size,
      GLboolean normalized,
      GLsizei stride,
      const void *pointer
    );

  public:
    Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indicies);
    ~Mesh();

    void draw() const;

    vector<Vertex> getVertices();

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) noexcept;
    Mesh &operator=(Mesh &&) noexcept;
};