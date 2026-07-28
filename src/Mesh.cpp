#include "Mesh.h"

Mesh::Mesh(const vector<Vertex> &vertices, const vector<unsigned int> &indicies)
: vertices(vertices),
  indices(indices) {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  const vector<float> vertexes = Vertex::flatten(vertices);

  glBufferData(
    GL_ARRAY_BUFFER,
    vertexes.size() * sizeof(float),
    vertexes.data(),
    GL_STATIC_DRAW
  );

  GLsizei stride = (vertexes.size() / vertices.size()) * sizeof(float);

  int currentIndex = 0;
  int offset = 3;
  addAttribute(0, 3, false, stride, (void *)0);

  if (vertices.at(0).hasColor()) {
    addAttribute(
      ++currentIndex, 4, GL_TRUE, stride, (void *)(offset * sizeof(float))
    );
    offset += 4;
  }

  if (vertices.at(0).hasUv()) {
    addAttribute(
      ++currentIndex, 2, GL_TRUE, stride, (void *)(offset * sizeof(float))
    );
    offset += 2;
  }

  if (vertices.at(0).hasNormal()) {
    addAttribute(
      ++currentIndex, 3, GL_TRUE, stride, (void *)(offset * sizeof(float))
    );
  }

  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    indices.size() * sizeof(indices.at(0)),
    indices.data(),
    GL_STATIC_DRAW
  );
}
Mesh::~Mesh() {
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Mesh::addAttribute(
  GLuint index,
  GLuint size,
  GLboolean normalized,
  GLsizei stride,
  const void *pointer
) {
  glVertexAttribPointer(index, size, GL_FLOAT, normalized, stride, pointer);
  glEnableVertexAttribArray(index);
}

void Mesh::draw() const {
  glBindVertexArray(VAO);

  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

vector<Vertex> Mesh::getVertices() {
  return vertices;
}