#include "MeshFactory.h"

#include <stdexcept>

MeshFactory::BoxBuilder::BoxBuilder(float width, float height, float depth)
: width(width),
  height(height),
  depth(depth) {
  if (width <= 0.0f || height <= 0.0f || depth <= 0.0f) {
    throw std::invalid_argument("Cube dimensions must be greater than zero");
  }
}

MeshFactory::BoxBuilder &MeshFactory::BoxBuilder::withUv() noexcept {
  includeUv = true;
  return *this;
}
MeshFactory::BoxBuilder &MeshFactory::BoxBuilder::withNormals() noexcept {
  includeNormals = true;
  return *this;
}

MeshFactory::BoxBuilder &
MeshFactory::BoxBuilder::withColor(const glm::vec4 &color) noexcept {
  includeColor = true;
  this->color = color;
  return *this;
}

Vertex MeshFactory::BoxBuilder::makeVertex(
  const glm::vec3 &position, const glm::vec2 &uv, const glm::vec3 &normal
) const {
  Vertex v(position);

  if (includeColor)
    v.setColor(color);

  if (includeUv)
    v.setUv(uv);

  if (includeNormals)
    v.setNormal(normal);

  return v;
}

Mesh MeshFactory::BoxBuilder::build() const {
  const float x = width * 0.5f;
  const float y = height * 0.5f;
  const float z = depth * 0.5f;

  const glm::vec2 uvs[4] = {
    {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}
  };

  const CubeFace faces[6] = {
    // Front
    {{{-x, -y, z}, {x, -y, z}, {x, y, z}, {-x, y, z}}, {0.0f, 0.0f, 1.0f}},

    // Back
    {{{x, -y, -z}, {-x, -y, -z}, {-x, y, -z}, {x, y, -z}}, {0.0f, 0.0f, -1.0f}},

    // Left
    {{{-x, -y, -z}, {-x, -y, z}, {-x, y, z}, {-x, y, -z}}, {-1.0f, 0.0f, 0.0f}},

    // Right
    {{{x, -y, z}, {x, -y, -z}, {x, y, -z}, {x, y, z}}, {1.0f, 0.0f, 0.0f}},

    // Top
    {{{-x, y, z}, {x, y, z}, {x, y, -z}, {-x, y, -z}}, {0.0f, 1.0f, 0.0f}},

    // Bottom
    {{{-x, -y, -z}, {x, -y, -z}, {x, -y, z}, {-x, -y, z}}, {0.0f, -1.0f, 0.0f}}
  };

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  vertices.reserve(24);
  indices.reserve(36);

  for (const CubeFace &face : faces) {
    const auto baseIndex = static_cast<unsigned int>(vertices.size());

    for (int i = 0; i < 4; i++) {
      vertices.push_back(makeVertex(face.positions[i], uvs[i], face.normal));
    }

    indices.insert(
      indices.end(),
      {
        baseIndex + 0,
        baseIndex + 1,
        baseIndex + 2,

        baseIndex + 2,
        baseIndex + 3,
        baseIndex + 0,
      }
    );
  }

  return Mesh(vertices, indices);
}

MeshFactory::BoxBuilder
MeshFactory::box(float width, float height, float depth) {
  return BoxBuilder(width, height, depth);
}