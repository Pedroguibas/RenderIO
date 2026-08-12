#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

#include <filesystem>
#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

struct ModelNode {
    glm::mat4 modelTransform;

    std::vector<size_t> meshIndices;
    std::vector<ModelNode> children;
};

class Model {
  private:
    std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>>
      loadedTextures;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::filesystem::path directory;
    ModelNode root;

  public:
    Model(std::vector<Mesh> meshes);
    Model(const std::filesystem::path &path);

    const std::vector<Mesh> &getMeshes() const noexcept;
    std::vector<Mesh> &getMeshes() noexcept;

    const std::vector<Material> &getMaterials() const noexcept;
    std::vector<Material> &getMaterials() noexcept;

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

    void processMaterials(const aiScene *scene);
    bool isPBRMaterial(const aiMaterial *material);
    Material processPBR(const aiMaterial *material, const aiScene *scene);
    Material processPhong(const aiMaterial *material, const aiScene *scene);

    void processAlbedo(
      const aiMaterial *material, const aiScene *scene, AlbedoOptions &options
    );
    void processRoughness(
      const aiMaterial *material,
      const aiScene *scene,
      RoughnessOptions &options
    );
    void processMetallic(
      const aiMaterial *material, const aiScene *scene, MetallicOptions &options
    );
    void processEmission(
      const aiMaterial *material, const aiScene *scene, EmissionOptions &options
    );
    void processSpecularity(
      const aiMaterial *material, const aiScene *scene, SpecularOptions &options
    );
    void processAmbientOcclusion(
      const aiMaterial *material,
      const aiScene *scene,
      AmbientOcclusionOptions &options
    );
    void processNormal(
      const aiMaterial *material, const aiScene *scene, NormalOptions &options
    );

    void processTextures(const aiMaterial *material);
    Texture processTexture(const aiMaterial *material);

    void drawNode(
      const ModelNode &node,
      Shader &shader,
      const glm::mat4 &transform = glm::mat4{1.0f}
    ) const;
    static glm::mat4 toGlmMat4(const aiMatrix4x4t<float> &m);
};