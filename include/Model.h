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
#include <optional>

enum class MaterialImportMode { Auto, PBR, Phong };

using NodeId = uint32_t;

struct ModelNode {
    std::string name;

    glm::mat4 modelTransform;

    std::vector<size_t> meshIndices;
    std::vector<NodeId> children;
};

class Model {
  private:
    std::unordered_map<std::filesystem::path, std::shared_ptr<Texture>>
      loadedTextures;
    std::vector<Material> materials;
    std::vector<Mesh> meshes;
    std::filesystem::path directory;
    std::vector<ModelNode> nodes;
    std::unordered_map<std::string, NodeId> nodesByName;
    NodeId rootNode;

  public:
    Model(std::vector<Mesh> meshes);
    Model(
      const std::filesystem::path &path,
      MaterialImportMode materialImportMode = MaterialImportMode::Auto
    );

    const std::vector<Mesh> &getMeshes() const noexcept;
    std::vector<Mesh> &getMeshes() noexcept;

    const std::vector<Material> &getMaterials() const noexcept;
    std::vector<Material> &getMaterials() noexcept;

    void draw(Shader &shader, const glm::mat4 &transform = glm::mat4{1.0f});

    ModelNode &getNode(const std::string &name);
    ModelNode &getNode(NodeId id);

  private:
    void loadModel(
      const std::filesystem::path &path,
      MaterialImportMode materialImportMode = MaterialImportMode::Auto
    );
    NodeId processNode(
      const aiNode *node,
      const aiScene *scene,
      const glm::mat4 &transform = {1.0f}
    );
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene);

    void processMaterials(
      const aiScene *scene,
      MaterialImportMode materialImportMode = MaterialImportMode::Auto
    );
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

    std::shared_ptr<Texture> processTexture(
      const aiMaterial *material, aiTextureType type, const aiScene *scene
    );

    void drawNode(
      const NodeId &nodeId,
      Shader &shader,
      const glm::mat4 &transform = glm::mat4{1.0f},
      std::optional<unsigned int> lastUsedMaterialIndex = std::nullopt
    );
    static glm::mat4 toGlmMat4(const aiMatrix4x4t<float> &m);
};