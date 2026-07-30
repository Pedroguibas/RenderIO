#pragma once

#include "Mesh.h"
#include "Vertex.h"
#include <glm/glm.hpp>

namespace {
  struct CubeFace {
      glm::vec3 positions[4];
      glm::vec3 normal;
  };
} // namespace

class MeshFactory {
  public:
    class BoxBuilder {
      private:
        float width;
        float height;
        float depth;

        bool includeUv = false;
        bool includeNormals = false;
        bool includeColor = false;

        glm::vec4 color{1.0f};

        Vertex makeVertex(
          const glm::vec3 &position,
          const glm::vec2 &uv,
          const glm::vec3 &normal
        ) const;

      public:
        BoxBuilder(float width, float height, float depth);

        BoxBuilder &withUv() noexcept;
        BoxBuilder &withNormals() noexcept;
        BoxBuilder &withColor(const glm::vec4 &color) noexcept;

        [[nodiscard]]
        Mesh build() const;
    };

    [[nodiscard]]
    static BoxBuilder
    box(float width = 1.0f, float height = 1.0f, float depth = 1.0f);
};