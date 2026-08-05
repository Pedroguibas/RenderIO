#pragma once

#include <glad/gl.h>
#include <vector>
#include "Vertex.h"
#include "Material.h"
#include "Shader.h"
using std::vector;

class Mesh {
  private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    Material material;
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
      const Material &material
    );
    ~Mesh();

    void draw(Shader &shader) const;

    const vector<Vertex> &getVertices() const noexcept;

    const Material &getMaterial() const noexcept;
    Material &getMaterial() noexcept;

    void setMaterial(const Material &material);

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;
    Mesh(Mesh &&) noexcept;
    Mesh &operator=(Mesh &&) noexcept;
};