#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#include <string>

using NodeId = uint32_t;

struct Transform {
    glm::vec3 translation{1.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f};
};

class ModelNode {
  private:
    std::string name;

    glm::mat4 localTransform{1.0f};
    Transform transform = {};

    std::optional<NodeId> parent;
    std::vector<size_t> meshIndices;
    std::vector<NodeId> children;

  public:
    ModelNode(
      std::string name,
      const glm::mat4 &localTransform = glm::mat4{1.0f},
      const std::vector<size_t> &meshIndices = {},
      const std::vector<NodeId> &children = {},
      std::optional<NodeId> parent = std::nullopt
    );

    const std::string &getName() const noexcept;

    const glm::mat4 &getLocalTransform() const noexcept;

    const std::vector<size_t> &getMeshIndices() const noexcept;
    void reserveMeshIndices(size_t size) noexcept;
    void pushMeshIndice(size_t indice) noexcept;

    const std::vector<NodeId> &getChildren() const noexcept;
    void reserveChildren(size_t size) noexcept;
    void pushChild(NodeId child) noexcept;

    const std::optional<NodeId> &getParent() const noexcept;
    void setParent(const std::optional<NodeId> &parent) noexcept;

    void translate(const glm::vec3 &offset);
    void rotateX(float rad);
    void rotateY(float rad);
    void rotateZ(float rad);
    void scale(const glm::vec3 &scale);
    void resetTranslation(const glm::vec3 &offset = glm::vec3{0.0f});
    void resetRotateX(float rad = 0);
    void resetRotateY(float rad = 0);
    void resetRotateZ(float rad = 0);
    void resetRotation();
    void resetScale(const glm::vec3 &scale = glm::vec3{1.0f});

    glm::mat4 getMatrix() const;
};
