#pragma once

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ModelNode.h"

#include <filesystem>
#include <vector>
#include <string>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

enum class MaterialImportMode { Auto, PBR, Phong };

struct Boundries {
    glm::vec3 min{0.0f};
    glm::vec3 max{0.0f};
    glm::vec3 pivotOffset{0.0f};
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
    Transform transform;
    Boundries bounds = {};

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

    void draw(Shader &shader);

    ModelNode &getNode(const std::string &name);
    ModelNode &getNode(NodeId id);

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
    const Transform &getTransform();

    glm::mat4 getTransformMatrix() const;

    const Boundries &getBounds();

  private:
    void loadModel(
      const std::filesystem::path &path,
      MaterialImportMode materialImportMode = MaterialImportMode::Auto
    );
    NodeId processNode(
      const aiNode *node,
      const aiScene *scene,
      std::optional<NodeId> parent = std::nullopt
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
      const glm::mat4 &parentTransform = glm::mat4{1.0f},
      std::optional<unsigned int> lastUsedMaterialIndex = std::nullopt
    );
    static glm::mat4 toGlmMat4(const aiMatrix4x4t<float> &m);

    std::shared_ptr<Texture> processEmbedded(const aiTexture *texture);

    void computeBounds();
    void accumulateBounds(NodeId id, const glm::mat4 &parentTransform);
};