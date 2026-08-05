#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <filesystem>
#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>

struct ModelNode {
    glm::mat4 modelTransform;

    std::vector<size_t> meshIndices;
    std::vector<ModelNode> children;
};

class Model {
  private:
    std::vector<Mesh> meshes;
    std::filesystem::path directory;
    ModelNode root;

  public:
    Model(std::vector<Mesh> meshes);
    Model(const std::filesystem::path &path);

    const std::vector<Mesh> &getMeshes() const noexcept;
    std::vector<Mesh> &getMeshes() noexcept;

    void
    draw(Shader &shader, const glm::mat4 &transform = glm::mat4{1.0f}) const;

  private:
    void loadModel(const std::filesystem::path &path);
    ModelNode processNode(
      const aiNode *node,
      const aiScene *scene,
      const glm::mat4 &transform = {1.0f}
    );
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
    void drawNode(
      const ModelNode &node,
      Shader &shader,
      const glm::mat4 &transform = glm::mat4{1.0f}
    ) const;
    static glm::mat4 toGlmMat4(const aiMatrix4x4t<float> &m);
};