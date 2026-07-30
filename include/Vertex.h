#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <vector>
using std::optional;
using std::vector;

class Vertex {
  private:
    glm::vec3 position;
    optional<glm::vec2> uv;
    optional<glm::vec4> color;
    optional<glm::vec3> normal;
    unsigned int length;

    void updateLength();

  public:
    Vertex(
      const glm::vec3 &position,
      const optional<glm::vec4> &color = std::nullopt,
      const optional<glm::vec2> &uv = std::nullopt,
      const optional<glm::vec3> &normal = std::nullopt
    );

    void setPosition(const glm::vec3 &position);
    glm::vec3 getPosition() const;

    void setUv(const optional<glm::vec2> &uv);
    optional<glm::vec2> getUv() const;

    void setColor(const optional<glm::vec4> &color);
    optional<glm::vec4> getColor() const;

    void setNormal(const optional<glm::vec3> &normal);
    optional<glm::vec3> getNormal() const;

    static vector<float> flatten(const vector<Vertex> &vertexes);

    vector<float> flatten();

    bool hasColor() const;
    bool hasUv() const;
    bool hasNormal() const;
};