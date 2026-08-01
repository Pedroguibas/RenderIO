#pragma once

#include "Shader.h"
#include <glm/glm.hpp>
using glm::vec3;
using glm::vec4;

class Light {
  private:
    vec4 color;

  public:
    Light(const vec4 &color);
    virtual ~Light() = default;

    const vec4 &getColor() const noexcept;
    void setColor(const vec4 &color) noexcept;

    virtual void upload(Shader &shader, const string &uniform) const = 0;
};

class DirectionLight : public Light {
  private:
    vec3 direction;

  public:
    DirectionLight(const vec4 &color, const vec3 &direction);

    const vec3 &getDirection() const noexcept;
    void setDirection(const vec3 &direction) noexcept;

    void upload(Shader &shader, const string &uniform) const override;
};

struct Attenuation {
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

class PointLight : public Light {
  private:
    vec3 position;

    Attenuation attenuation;

  public:
    PointLight(
      const vec4 &color,
      const vec3 &position,
      const Attenuation &attenuation = {}
    );

    const vec3 &getPosition() const noexcept;
    void setPosition(const vec3 &position) noexcept;

    Attenuation getAttenuation() const noexcept;
    void setAttenuation(const Attenuation &attenuation) noexcept;

    void upload(Shader &shader, const string &uniform) const override;
};

struct SpotLightSettings {
    Attenuation attenuation;
    float innerCutoffDeg = 12.5f;
    float outerCutoffDeg = 17.5f;
};

class SpotLight : public PointLight {
  private:
    float innerCutoffDeg;
    float outerCutoffDeg;
    float innerCutoffCos = glm::cos(glm::radians(innerCutoffDeg));
    float outerCutoffCos = glm::cos(glm::radians(outerCutoffDeg));
    vec3 direction;

  public:
    SpotLight(
      const vec4 &color,
      const vec3 &position,
      const vec3 &direction,
      const SpotLightSettings &settings = {}
    );

    const vec3 &getDirection() const noexcept;
    void setDirection(const vec3 &direction) noexcept;

    void setInnerCutoffDeg(const float deg) noexcept;
    void setOuterCutoffDeg(const float deg) noexcept;

    float getInnerCutoffDeg() const noexcept;
    float getOuterCutoffDeg() const noexcept;

    float getInnerCutoffCos() const noexcept;
    float getOuterCutoffCos() const noexcept;

    void upload(Shader &shader, const string &uniform) const override;
};