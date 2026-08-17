#include "ModelNode.h"

ModelNode::ModelNode(
  std::string name,
  const glm::mat4 &localTransform,
  const std::vector<size_t> &meshIndices,
  const std::vector<NodeId> &children,
  std::optional<NodeId> parent
) {
  this->name = name;
  this->localTransform = localTransform;
  this->meshIndices = meshIndices;
  this->children = children;
  this->parent = parent;
}

const std::string &ModelNode::getName() const noexcept {
  return name;
}

const glm::mat4 &ModelNode::getLocalTransform() const noexcept {
  return localTransform;
}

const std::vector<size_t> &ModelNode::getMeshIndices() const noexcept {
  return meshIndices;
}
void ModelNode::reserveMeshIndices(size_t size) noexcept {
  meshIndices.reserve(size);
}
void ModelNode::pushMeshIndice(size_t indice) noexcept {
  meshIndices.push_back(indice);
}

const std::vector<NodeId> &ModelNode::getChildren() const noexcept {
  return children;
}
void ModelNode::reserveChildren(size_t size) noexcept {
  children.reserve(size);
}
void ModelNode::pushChild(NodeId child) noexcept {
  children.push_back(child);
}

const std::optional<NodeId> &ModelNode::getParent() const noexcept {
  return parent;
}
void ModelNode::setParent(const std::optional<NodeId> &parent) noexcept {
  this->parent = parent;
}

void ModelNode::translate(const glm::vec3 &offset) {
  transform.translation += offset;
}
void ModelNode::rotateX(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{1.0f, 0.0f, 0.0f});
}
void ModelNode::rotateY(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{0.0f, 1.0f, 0.0f});
}
void ModelNode::rotateZ(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{0.0f, 0.0f, 1.0f});
}
void ModelNode::scale(const glm::vec3 &scale) {
  transform.scale *= scale;
}
void ModelNode::resetTranslation(const glm::vec3 &offset) {
  transform.translation = offset;
}
void ModelNode::resetRotateX(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{1.0f, 0.0f, 0.0f});
}
void ModelNode::resetRotateY(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{0.0f, 1.0f, 0.0f});
}
void ModelNode::resetRotateZ(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{0.0f, 0.0f, 1.0f});
}
void ModelNode::resetRotation() {
  transform.rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};
}
void ModelNode::resetScale(const glm::vec3 &scale) {
  transform.scale = scale;
}

glm::mat4 ModelNode::getMatrix() const {
  const glm::mat4 T = glm::translate(glm::mat4{1.0f}, transform.translation);

  const glm::mat4 R = glm::mat4_cast(transform.rotation);

  const glm::mat4 S = glm::scale(glm::mat4{1.0f}, transform.scale);

  return localTransform * T * R * S;
}