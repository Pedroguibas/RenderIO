#include "Lights.h"

#include <algorithm>
#include <string>

Light::Light(const vec4 &color) : color(color) {}

const vec4 &Light::getColor() const noexcept {
  return color;
}
void Light::setColor(const vec4 &color) noexcept {
  this->color = color;
}

// Direction Light

DirectionLight::DirectionLight(const vec4 &color, const vec3 &direction)
: Light(color),
  direction(glm::normalize(direction)) {}

const vec3 &DirectionLight::getDirection() const noexcept {
  return direction;
}
void DirectionLight::setDirection(const vec3 &direction) noexcept {
  this->direction = glm::normalize(direction);
}

void DirectionLight::upload(Shader &shader, const string &uniform) const {
  const string base = uniform + '.';

  shader.setVec4(base + "color", getColor());
  shader.setVec3(base + "direction", getDirection());
}

// Point Light

PointLight::PointLight(
  const vec4 &color, const vec3 &position, const Attenuation &attenuation
)
: Light(color),
  position(position),
  attenuation(attenuation) {}

const vec3 &PointLight::getPosition() const noexcept {
  return position;
}
void PointLight::setPosition(const vec3 &position) noexcept {
  this->position = position;
}

Attenuation PointLight::getAttenuation() const noexcept {
  return attenuation;
}
void PointLight::setAttenuation(const Attenuation &attenuation) noexcept {
  this->attenuation = attenuation;
}

void PointLight::upload(Shader &shader, const string &uniform) const {
  const string base = uniform + '.';

  shader.setVec4(base + "color", getColor());
  shader.setVec3(base + "position", getPosition());
  shader.setFloat(base + "constant", getAttenuation().constant);
  shader.setFloat(base + "linear", getAttenuation().linear);
  shader.setFloat(base + "quadratic", getAttenuation().quadratic);
}

// SpotLight

SpotLight::SpotLight(
  const vec4 &color,
  const vec3 &position,
  const vec3 &direction,
  const SpotLightSettings &settings
)
: PointLight(color, position, settings.attenuation),
  direction(glm::normalize(direction)) {
  setInnerCutoffDeg(settings.innerCutoffDeg);
  setOuterCutoffDeg(settings.outerCutoffDeg);
}

const vec3 &SpotLight::getDirection() const noexcept {
  return direction;
}
void SpotLight::setDirection(const vec3 &direction) noexcept {
  this->direction = glm::normalize(direction);
}

void SpotLight::setInnerCutoffDeg(const float deg) noexcept {
  innerCutoffDeg = std::clamp(deg, 0.0f, outerCutoffDeg);
  innerCutoffCos = glm::cos(glm::radians(innerCutoffDeg));
}
void SpotLight::setOuterCutoffDeg(const float deg) noexcept {
  outerCutoffDeg = std::clamp(deg, innerCutoffDeg, 90.0f);
  outerCutoffCos = glm::cos(glm::radians(outerCutoffDeg));
}

float SpotLight::getInnerCutoffDeg() const noexcept {
  return innerCutoffDeg;
}
float SpotLight::getOuterCutoffDeg() const noexcept {
  return outerCutoffDeg;
}

float SpotLight::getInnerCutoffCos() const noexcept {
  return innerCutoffCos;
}
float SpotLight::getOuterCutoffCos() const noexcept {
  return outerCutoffCos;
}

void SpotLight::upload(Shader &shader, const string &uniform) const {
  const string base = uniform + '.';

  shader.setVec4(base + "color", PointLight::getColor());
  shader.setVec3(base + "position", getPosition());
  shader.setVec3(base + "direction", getDirection());
  shader.setFloat(base + "constant", getAttenuation().constant);
  shader.setFloat(base + "linear", getAttenuation().linear);
  shader.setFloat(base + "quadratic", getAttenuation().quadratic);
  shader.setFloat(base + "outerCutoff", getOuterCutoffCos());
  shader.setFloat(base + "innerCutoff", getInnerCutoffCos());
}