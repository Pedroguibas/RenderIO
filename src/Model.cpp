#include "Model.h"

Model::Model(const std::vector<Mesh> &meshes) : meshes(std::move(meshes)) {}

const std::vector<Mesh> &Model::getMeshes() const noexcept {
  return meshes;
}
std::vector<Mesh> &Model::getMeshes() noexcept {
  return meshes;
}

void Model::draw(Shader &shader) const {
  for (const auto &mesh : meshes) {
    mesh.draw(shader);
  }
}