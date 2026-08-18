#include "Model.h"

#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stdexcept>
#include <stb_image.h>

Model::Model(std::vector<Mesh> meshes) : meshes(std::move(meshes)) {
  ModelNode root("root");

  root.reserveMeshIndices(meshes.size());

  rootNode = nodes.size();
  for (std::size_t i = 0; i < meshes.size(); i++) {
    root.pushMeshIndice(static_cast<unsigned int>(i));
  }

  nodes.push_back(root);
}

Model::Model(
  const std::filesystem::path &path, MaterialImportMode materialImportMode
)
: directory(path) {
  loadModel(path, materialImportMode);
}

const std::vector<Mesh> &Model::getMeshes() const noexcept {
  return meshes;
}
std::vector<Mesh> &Model::getMeshes() noexcept {
  return meshes;
}

void Model::draw(Shader &shader) {
  drawNode(rootNode, shader, getTransformMatrix());
}

void Model::loadModel(
  const std::filesystem::path &path, MaterialImportMode materialImportMode
) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
    path.string(),
    aiProcess_Triangulate | aiProcess_GenSmoothNormals |
      aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace
  );

  if (
    scene == nullptr || scene->mRootNode == nullptr ||
    (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
  ) {
    throw std::runtime_error(
      "Failed to load model '" + path.string() +
      "': " + importer.GetErrorString()
    );
  }

  directory = path.parent_path();

  meshes.clear();
  rootNode = processNode(scene->mRootNode, scene);
  processMaterials(scene);
}

NodeId Model::processNode(
  const aiNode *node, const aiScene *scene, std::optional<NodeId> parent
) {
  NodeId id = static_cast<NodeId>(nodes.size());

  ModelNode modelNode(
    node->mName.C_Str(), Model::toGlmMat4(node->mTransformation)
  );

  modelNode.setParent(parent);

  modelNode.reserveMeshIndices(node->mNumMeshes);
  modelNode.reserveChildren(node->mNumChildren);

  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    const unsigned int meshIndex = node->mMeshes[i];
    const aiMesh *mesh = scene->mMeshes[meshIndex];

    modelNode.pushMeshIndice(meshes.size());
    meshes.push_back(processMesh(mesh, scene));
  }

  nodes.push_back(modelNode);

  nodesByName.emplace(modelNode.getName(), id);

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    NodeId childId = processNode(node->mChildren[i], scene, id);

    nodes[id].pushChild(childId);
  }

  return id;
}

Mesh Model::processMesh(const aiMesh *mesh, const aiScene *scene) {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  vertices.reserve(mesh->mNumVertices);

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex(
      glm::vec3(
        mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z
      )
    );

    if (mesh->HasNormals()) {
      vertex.setNormal(
        glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
      );
    }

    if (mesh->HasTextureCoords(0)) {
      vertex.setUv(
        glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
      );
    }

    if (mesh->HasTangentsAndBitangents()) {
      aiVector3D N = mesh->mNormals[i];
      aiVector3D T = mesh->mTangents[i];
      aiVector3D B = mesh->mBitangents[i];

      float w = ((N ^ T) * B) < 0.0f ? -1.0f : 1.0f;

      vertex.setTangent(glm::vec4(T.x, T.y, T.z, w));
    }

    vertices.push_back(vertex);
  }

  indices.reserve(mesh->mNumFaces);
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const aiFace &face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  Material material = Material::createPhong({});

  return Mesh(std::move(vertices), std::move(indices), mesh->mMaterialIndex);
}

void Model::drawNode(
  const NodeId &nodeId,
  Shader &shader,
  const glm::mat4 &parentTransform,
  std::optional<unsigned int> lastUsedMaterialIndex
) {
  ModelNode node = nodes[nodeId];
  glm::mat4 worldTransform = parentTransform * node.getMatrix();
  shader.setMat4("model", worldTransform);

  for (auto idx : node.getMeshIndices()) {
    const unsigned int materialIndex = meshes[idx].getMaterialIndex();
    if (!lastUsedMaterialIndex || materialIndex != *lastUsedMaterialIndex) {
      materials[materialIndex].use(shader);
      lastUsedMaterialIndex = materialIndex;
    }

    meshes[idx].draw();
  }

  for (const auto &n : node.getChildren())
    drawNode(n, shader, worldTransform, lastUsedMaterialIndex);
}

glm::mat4 Model::toGlmMat4(const aiMatrix4x4t<float> &m) {
  return glm::mat4{
    {m.a1, m.b1, m.c1, m.d1},
    {m.a2, m.b2, m.c2, m.d2},
    {m.a3, m.b3, m.c3, m.d3},
    {m.a4, m.b4, m.c4, m.d4},
  };
}

void Model::processMaterials(
  const aiScene *scene, MaterialImportMode materialImportMode
) {
  switch (materialImportMode) {
  case MaterialImportMode::Auto:
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
      if (isPBRMaterial(scene->mMaterials[i]))
        materials.push_back(processPBR(scene->mMaterials[i], scene));
      else
        materials.push_back(processPhong(scene->mMaterials[i], scene));
    }
    break;

  case MaterialImportMode::PBR:
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
      materials.push_back(processPBR(scene->mMaterials[i], scene));
    break;

  case MaterialImportMode::Phong:
    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
      materials.push_back(processPhong(scene->mMaterials[i], scene));
    break;
  }
}

bool Model::isPBRMaterial(const aiMaterial *material) {
  aiShadingMode shadingMode;

  if (
    material->Get(AI_MATKEY_SHADING_MODEL, shadingMode) == AI_SUCCESS &&
    shadingMode == aiShadingMode_PBR_BRDF
  ) {
    return true;
  }

  float value;

  if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
    return true;

  if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
    return true;

  if (material->GetTextureCount(aiTextureType_METALNESS) > 0)
    return true;

  if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
    return true;

  return false;
}

Material Model::processPBR(const aiMaterial *material, const aiScene *scene) {
  PBRShaderOptions options;

  processAlbedo(material, scene, options.albedo);
  processRoughness(material, scene, options.roughness);
  processMetallic(material, scene, options.metallic);
  processAmbientOcclusion(material, scene, options.ao);
  processNormal(material, scene, options.normal);
  processEmission(material, scene, options.emission);

  return Material::createPBR(options);
}

Material Model::processPhong(const aiMaterial *material, const aiScene *scene) {
  PhongShaderOptions options;

  processAlbedo(material, scene, options.albedo);
  processSpecularity(material, scene, options.specular);
  processEmission(material, scene, options.emission);

  return Material::createPhong(options);
}

void Model::processAlbedo(
  const aiMaterial *material, const aiScene *scene, AlbedoOptions &options
) {
  aiColor4D color;

  if (material->Get(AI_MATKEY_BASE_COLOR, color) == AI_SUCCESS) {
    options.baseColor = glm::vec4{color.r, color.g, color.b, color.a};
  }

  if (material->GetTextureCount(aiTextureType_BASE_COLOR) > 0) {
    auto texture = processTexture(material, aiTextureType_BASE_COLOR, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
  }
}
void Model::processRoughness(
  const aiMaterial *material, const aiScene *scene, RoughnessOptions &options
) {
  float value;

  if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS) {
    options.value = value;
  }

  if (material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0) {
    auto texture =
      processTexture(material, aiTextureType_DIFFUSE_ROUGHNESS, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
  }
}
void Model::processMetallic(
  const aiMaterial *material, const aiScene *scene, MetallicOptions &options
) {
  float value;

  if (material->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS) {
    options.value = value;
  }

  if (material->GetTextureCount(aiTextureType_METALNESS) > 0) {
    auto texture = processTexture(material, aiTextureType_METALNESS, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
  }
}
void Model::processEmission(
  const aiMaterial *material, const aiScene *scene, EmissionOptions &options
) {
  float value;

  if (material->Get(AI_MATKEY_EMISSIVE_INTENSITY, value) == AI_SUCCESS) {
    options.intensity = value;

    options.enabled = true;
  }

  if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
    auto texture = processTexture(material, aiTextureType_EMISSIVE, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
      options.enabled = true;
    }
  }
}
void Model::processSpecularity(
  const aiMaterial *material, const aiScene *scene, SpecularOptions &options
) {
  float value;

  if (material->Get(AI_MATKEY_SPECULAR_FACTOR, value) == AI_SUCCESS) {
    options.intensity = value;

    aiColor3D color;
    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
      options.color = glm::vec3{color.r, color.g, color.b};
    }

    if (material->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS) {
      options.shininess = value;
    }
    options.enabled = true;
  }

  if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
    auto texture = processTexture(material, aiTextureType_SPECULAR, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
    options.enabled = true;
  }
}
void Model::processAmbientOcclusion(
  const aiMaterial *material,
  const aiScene *scene,
  AmbientOcclusionOptions &options
) {
  options.value = 1.0f;

  if (material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0) {
    auto texture =
      processTexture(material, aiTextureType_AMBIENT_OCCLUSION, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
  }
}
void Model::processNormal(
  const aiMaterial *material, const aiScene *scene, NormalOptions &options
) {
  if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
    auto texture = processTexture(material, aiTextureType_NORMALS, scene);

    if (texture) {
      options.texture = texture;
      options.mapEnabled = true;
    }
  }
}

std::shared_ptr<Texture> Model::processTexture(
  const aiMaterial *material, aiTextureType type, const aiScene *scene
) {
  if (material->GetTextureCount(type) == 0)
    return 0;

  aiString texPath;

  if (material->GetTexture(type, 0, &texPath) != AI_SUCCESS)
    return 0;

  std::string path = texPath.C_Str();

  if (const aiTexture *embedded = scene->GetEmbeddedTexture(path.c_str())) {
    std::string key = "embedded:" + path;

    auto it = loadedTextures.find(key);
    if (it != loadedTextures.end())
      return it->second;

    auto texture = processEmbedded(embedded);

    loadedTextures.emplace(key, texture);

    return texture;
  }

  std::filesystem::path relativePath(path);
  std::filesystem::path fullPath =
    (directory / relativePath).lexically_normal();

  auto it = loadedTextures.find(fullPath);

  if (it != loadedTextures.end()) {
    return it->second;
  }

  auto texture = std::make_shared<Texture>(fullPath);

  loadedTextures.emplace(fullPath, texture);

  return texture;
}

std::shared_ptr<Texture> Model::processEmbedded(const aiTexture *embedded) {
  if (embedded->mHeight == 0) {
    int width;
    int height;
    int channels;

    const auto *buffer =
      reinterpret_cast<const unsigned char *>(embedded->pcData);

    unsigned char *data = stbi_load_from_memory(
      buffer, static_cast<int>(embedded->mWidth), &width, &height, &channels, 0
    );

    if (!data) {
      throw std::runtime_error(
        std::string("Failed to load embedded texture:\n") +
        stbi_failure_reason()
      );
    }

    auto texture = std::make_shared<Texture>(data, width, height, channels);

    stbi_image_free(data);

    return texture;
  }

  return std::make_shared<Texture>(
    embedded->pcData, embedded->mWidth, embedded->mHeight, GL_BGRA, GL_RGBA
  );
}

ModelNode &Model::getNode(const std::string &name) {
  try {
    return nodes.at(nodesByName.at(name));
  } catch (const std::exception &e) {
    return nodes[rootNode];
  }
}
ModelNode &Model::getNode(NodeId id) {
  return nodes.at(id);
}

void Model::translate(const glm::vec3 &offset) {
  transform.translation += offset;
}
void Model::rotateX(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{1.0f, 0.0f, 0.0f});
}
void Model::rotateY(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{0.0f, 1.0f, 0.0f});
}
void Model::rotateZ(float rad) {
  transform.rotation *= glm::angleAxis(rad, glm::vec3{0.0f, 0.0f, 1.0f});
}
void Model::scale(const glm::vec3 &scale) {
  transform.scale *= scale;
}
void Model::resetTranslation(const glm::vec3 &offset) {
  transform.translation = offset;
}
void Model::resetRotateX(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{1.0f, 0.0f, 0.0f});
}
void Model::resetRotateY(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{0.0f, 1.0f, 0.0f});
}
void Model::resetRotateZ(float rad) {
  transform.rotation = glm::angleAxis(rad, glm::vec3{0.0f, 0.0f, 1.0f});
}
void Model::resetRotation() {
  transform.rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f};
}
void Model::resetScale(const glm::vec3 &scale) {
  transform.scale = scale;
}

glm::mat4 Model::getTransformMatrix() const {
  const glm::mat4 T = glm::translate(glm::mat4{1.0f}, transform.translation);

  const glm::mat4 R = glm::mat4_cast(transform.rotation);

  const glm::mat4 S = glm::scale(glm::mat4{1.0f}, transform.scale);

  return T * R * S;
}