#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <filesystem>
#include <vector>
#include <string>
#include <assimp/scene.h>

class Model {
  private:
    std::vector<Mesh> meshes;
    std::filesystem::path directory;

  public:
    Model(const std::vector<Mesh> &meshes);
    Model(const std::filesystem::path &path);

    const std::vector<Mesh> &getMeshes() const noexcept;
    std::vector<Mesh> &getMeshes() noexcept;

    void draw(Shader &shader) const;

  private:
    void loadModel(const std::filesystem::path &path);
    void processNode(const aiNode *node, const aiScene *scene);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);
};