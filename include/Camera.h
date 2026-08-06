#pragma once

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

class Camera {
  private:
    vec3 position;

    float yaw;
    float pitch;

    vec3 forward;
    vec3 up;
    vec3 right;

    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    float speed = 0.5f;
    float mouseSensitivity = 0.0003f;

    void setDirection(const vec3 &dir);
    void updateVectors();
    const glm::vec3 calculateMovement(glm::vec3 movement);

  public:
    Camera();
    Camera(const vec3 &position);
    Camera(const vec3 &position, const float yaw, const float pitch);

    void lookAt(const vec3 &focus);

    void setPosition(const vec3 &position);
    vec3 getPosition() const;

    mat4 getView() const;
    mat4 getPerspective(const float aspect) const;

    float getSpeed() const noexcept;
    void setSpeed(float speed) noexcept;

    void moveForward(float deltaTime) noexcept;
    void moveBackwards(float deltaTime) noexcept;
    void moveRight(float deltaTime) noexcept;
    void moveLeft(float deltaTime) noexcept;
    void moveUp(float deltaTime) noexcept;
    void moveDown(float deltaTime) noexcept;

    void rotate(float x, float y) noexcept;
};