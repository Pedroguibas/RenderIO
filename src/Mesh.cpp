#include "Mesh.h"

#include <stdexcept>

Mesh::Mesh(
  const vector<Vertex> &vertices,
  const vector<unsigned int> &indices,
  unsigned int materialIndex
)
: vertices(vertices),
  indices(indices),
  materialIndex(materialIndex) {

  if (vertices.empty()) {
    throw std::invalid_argument("Mesh requires at least one vertex");
  }

  if (indices.empty()) {
    throw std::invalid_argument("Mesh requires at least one index");
  }

  if (
    indices.size() >
    static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())
  ) {
    throw std::overflow_error("Mesh has too many indices");
  }

  const Vertex &first = vertices.front();

  for (const Vertex &vertex : vertices) {
    if (
      vertex.hasColor() != first.hasColor() ||
      vertex.hasUv() != first.hasUv() ||
      vertex.hasNormal() != first.hasNormal() ||
      vertex.hasTangent() != first.hasTangent()
    ) {
      throw std::invalid_argument(
        "All mesh vertices must have the same layout"
      );
    }
  }

  indiceCount = static_cast<GLsizei>(indices.size());

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

  int offset = 3;
  addAttribute(0, 3, GL_FALSE, stride, (void *)0);

  if (vertices.at(0).hasColor()) {
    addAttribute(1, 4, GL_FALSE, stride, (void *)(offset * sizeof(float)));
    offset += 4;
  }

  if (vertices.at(0).hasUv()) {
    addAttribute(2, 2, GL_FALSE, stride, (void *)(offset * sizeof(float)));
    offset += 2;
  }

  if (vertices.at(0).hasNormal()) {
    addAttribute(3, 3, GL_FALSE, stride, (void *)(offset * sizeof(float)));
    offset += 3;
  }

  if (vertices.at(0).hasTangent()) {
    addAttribute(4, 3, GL_FALSE, stride, (void *)(offset * sizeof(float)));
    offset += 3;
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

unsigned int Mesh::getMaterialIndex() const noexcept {
  return materialIndex;
}

void Mesh::setMaterialIndex(unsigned int idx) {
  materialIndex = idx;
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

  glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

const vector<Vertex> &Mesh::getVertices() const {
  return vertices;
}

Mesh::Mesh(Mesh &&other) noexcept
: VAO(other.VAO),
  VBO(other.VBO),
  EBO(other.EBO),
  indiceCount(other.indiceCount),
  vertices(std::move(other.vertices)),
  indices(std::move(other.indices)),
  materialIndex(other.materialIndex) {

  other.VAO = 0;
  other.VBO = 0;
  other.EBO = 0;
  other.indiceCount = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
  if (this == &other) {
    return *this;
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  VAO = other.VAO;
  VBO = other.VBO;
  EBO = other.EBO;
  indiceCount = other.indiceCount;

  vertices = std::move(other.vertices);
  indices = std::move(other.indices);
  materialIndex = other.materialIndex;

  other.VAO = 0;
  other.VBO = 0;
  other.EBO = 0;
  other.indiceCount = 0;

  return *this;
}