#include "Vertex.h"

#include <stdexcept>

Vertex::Vertex(
  const glm::vec3 &position,
  const optional<glm::vec4> &color,
  const optional<glm::vec2> &uv,
  const optional<glm::vec3> &normal,
  const std::optional<glm::vec4> &tangent
)
: position(position),
  uv(uv),
  color(color),
  normal(normal),
  tangent(tangent) {

  updateLength();
}

void Vertex::updateLength() {
  length = 3;

  if (uv)
    length += 2;

  if (color)
    length += 4;

  if (normal)
    length += 3;

  if (tangent)
    length += 4;
}

void Vertex::setPosition(const glm::vec3 &position) {
  this->position = position;
  updateLength();
}
glm::vec3 Vertex::getPosition() const {
  return position;
}

void Vertex::setUv(const optional<glm::vec2> &uv) {
  this->uv = uv;
  updateLength();
}
optional<glm::vec2> Vertex::getUv() const {
  return uv;
}

void Vertex::setColor(const optional<glm::vec4> &color) {
  this->color = color;
  updateLength();
}
optional<glm::vec4> Vertex::getColor() const {
  return color;
}

void Vertex::setNormal(const optional<glm::vec3> &normal) {
  if (normal.has_value())
    this->normal = glm::normalize(normal.value());
  else
    this->normal = normal;

  updateLength();
}
optional<glm::vec3> Vertex::getNormal() const {
  return normal;
}

void Vertex::setTangent(const std::optional<glm::vec4> &tangent) {
  if (tangent.has_value())
    this->tangent = glm::normalize(tangent.value());
  else
    this->tangent = tangent;

  updateLength();
}
std::optional<glm::vec4> Vertex::getTangent() const {
  return tangent;
}

vector<float> Vertex::flatten() {
  vector<float> vec;

  vec.push_back(position.x);
  vec.push_back(position.y);
  vec.push_back(position.z);

  if (hasColor()) {
    vec.push_back(color->x);
    vec.push_back(color->y);
    vec.push_back(color->z);
    vec.push_back(color->w);
  }

  if (hasUv()) {
    vec.push_back(uv->x);
    vec.push_back(uv->y);
  }

  if (hasNormal()) {
    vec.push_back(normal->x);
    vec.push_back(normal->y);
    vec.push_back(normal->z);
  }

  if (hasTangent()) {
    vec.push_back(tangent->x);
    vec.push_back(tangent->y);
    vec.push_back(tangent->z);
    vec.push_back(tangent->w);
  }

  return vec;
}

vector<float> Vertex::flatten(const vector<Vertex> &vertexes) {
  vector<float> vec;
  for (Vertex vertex : vertexes) {
    vec.push_back(vertex.position.x);
    vec.push_back(vertex.position.y);
    vec.push_back(vertex.position.z);

    if (vertex.hasColor()) {
      vec.push_back(vertex.color->x);
      vec.push_back(vertex.color->y);
      vec.push_back(vertex.color->z);
      vec.push_back(vertex.color->w);
    }

    if (vertex.hasUv()) {
      vec.push_back(vertex.uv->x);
      vec.push_back(vertex.uv->y);
    }

    if (vertex.hasNormal()) {
      vec.push_back(vertex.normal->x);
      vec.push_back(vertex.normal->y);
      vec.push_back(vertex.normal->z);
    }
  }

  return vec;
}

bool Vertex::hasColor() const {
  return color.has_value();
}
bool Vertex::hasUv() const {
  return uv.has_value();
}
bool Vertex::hasNormal() const {
  return normal.has_value();
}
bool Vertex::hasTangent() const {
  return tangent.has_value();
}

glm::vec4 Vertex::genTangent(
  const glm::vec3 &p1,
  const glm::vec3 &p2,
  const glm::vec3 &p3,
  const glm::vec2 &uv1,
  const glm::vec2 &uv2,
  const glm::vec2 &uv3,
  const glm::vec3 &N
) {

  glm::vec3 E1 = p2 - p1;
  glm::vec3 E2 = p3 - p1;

  glm::vec2 deltaUV1 = uv2 - uv1;
  glm::vec2 deltaUV2 = uv3 - uv1;

  float det = deltaUV1.x * deltaUV2.y - deltaUV1.x * deltaUV2.y;

  if (std::abs(det) < 1e-6f) {
    glm::vec3 axis = std::abs(N.x) < 0.9f ? glm::vec3(1.0f, 0.0f, 0.0f)
                                          : glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::vec4(glm::normalize(glm::cross(axis, N)), 1.0f);
  }

  float f = 1.0f / det;

  glm::vec3 T;

  T = f * (deltaUV2.y * E1 - deltaUV1.y * E2);

  glm::vec3 B = f * (deltaUV2.y * E1 - deltaUV1.x * E2);

  float handedness = glm::dot(glm::cross(N, T), B) < 0.0f ? -1.0f : 1.0f;

  return glm::vec4(glm::normalize(T), handedness);
}