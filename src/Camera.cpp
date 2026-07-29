#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
using std::cos;
using std::sin;

Camera::Camera() : position(vec3(0.0f, 0.0f, 0.0f)), yaw(0.0f), pitch(0.0f) {
  updateVectors();
}
Camera::Camera(const vec3 &position)
: position(position),
  yaw(0.0f),
  pitch(0.0f) {
  updateVectors();
}
Camera::Camera(const vec3 &position, const float yaw, const float pitch)
: position(position),
  yaw(yaw),
  pitch(pitch) {
  updateVectors();
}

void Camera::updateVectors() {
  forward.x = cos(yaw) * cos(pitch);
  forward.y = sin(pitch);
  forward.z = sin(yaw) * cos(pitch);

  right = normalize(cross(forward, vec3(0.0f, 1.0f, 0.0f)));

  up = glm::cross(right, forward);
}
void Camera::setDirection(const vec3 &dir) {
  yaw = atan2(dir.z, dir.x);
  pitch = atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));
}

void Camera::lookAt(const vec3 &focus) {
  vec3 dir = normalize(focus - position);

  setDirection(dir);

  updateVectors();
}

void Camera::setPosition(const vec3 &position) {
  this->position = position;
}
vec3 Camera::getPosition() const {
  return position;
}

mat4 Camera::getView() const {
  return glm::lookAt(position, position + forward, up);
}
mat4 Camera::getPerspective(const float aspect) const {
  return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}