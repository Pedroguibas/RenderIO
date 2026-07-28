#include "Vertex.h"

using std::nullopt;

Vertex::Vertex(
  const glm::vec3 &position,
  const optional<glm::vec4> &color,
  const optional<glm::vec2> &uv,
  const optional<glm::vec3> &normal
)
: position(position),
  uv(uv),
  color(color),
  normal(normal) {

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
  this->normal = normal;
  updateLength();
}
optional<glm::vec3> Vertex::getNormal() const {
  return normal;
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