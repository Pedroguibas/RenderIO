#include "Model.h"

#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stdexcept>

Model::Model(std::vector<Mesh> meshes) : meshes(std::move(meshes)) {
  ModelNode root;

  root.modelTransform = glm::mat4{1.0f};

  root.meshIndices.reserve(meshes.size());

  for (std::size_t i = 0; i < meshes.size(); i++) {
    root.meshIndices.push_back(static_cast<unsigned int>(i));
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

void Model::draw(Shader &shader, const glm::mat4 &transform) {
  drawNode(rootNode, shader, transform);
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
  const aiNode *node, const aiScene *scene, const glm::mat4 &transform
) {
  ModelNode modelNode;
  modelNode.meshIndices.reserve(node->mNumMeshes);
  modelNode.children.reserve(node->mNumChildren);
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    const unsigned int meshIndex = node->mMeshes[i];
    const aiMesh *mesh = scene->mMeshes[meshIndex];

    meshes.push_back(processMesh(mesh, scene));
    modelNode.meshIndices.push_back(meshes.size() - 1);
  }
  modelNode.name = node->mName.C_Str();

  modelNode.modelTransform =
    transform * Model::toGlmMat4(node->mTransformation);

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    modelNode.children.push_back(
      processNode(node->mChildren[i], scene, modelNode.modelTransform)
    );
  }

  NodeId id = static_cast<NodeId>(nodes.size());
  nodesByName.emplace(modelNode.name, id);

  nodes.push_back(modelNode);
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
  const glm::mat4 &transform,
  std::optional<unsigned int> lastUsedMaterialIndex
) {
  ModelNode node = nodes[nodeId];
  shader.setMat4("model", transform * node.modelTransform);

  for (auto idx : node.meshIndices) {
    const unsigned int materialIndex = meshes[idx].getMaterialIndex();
    if (!lastUsedMaterialIndex || materialIndex != *lastUsedMaterialIndex) {
      materials[materialIndex].use(shader);
      lastUsedMaterialIndex = materialIndex;
    }

    meshes[idx].draw();
  }

  for (const auto &n : node.children)
    drawNode(n, shader, transform);
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

  const char *path = texPath.C_Str();

  if (const aiTexture *embedded = scene->GetEmbeddedTexture(path)) {
    throw std::runtime_error("Embedded Textures are not supported yet");
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

ModelNode &Model::getNode(const std::string &name) {
  return nodes.at(nodesByName[name]);
}
ModelNode &Model::getNode(NodeId id) {
  return nodes.at(id);
}