#include "Model.h"

#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stdexcept>

Model::Model(std::vector<Mesh> meshes) : meshes(std::move(meshes)) {
  root.modelTransform = glm::mat4{1.0f};

  root.meshIndices.reserve(meshes.size());

  for (std::size_t i = 0; i < meshes.size(); i++) {
    root.meshIndices.push_back(static_cast<unsigned int>(i));
  }
}

Model::Model(const std::filesystem::path &path) : directory(path) {
  loadModel(path);
}

const std::vector<Mesh> &Model::getMeshes() const noexcept {
  return meshes;
}
std::vector<Mesh> &Model::getMeshes() noexcept {
  return meshes;
}

void Model::draw(Shader &shader, const glm::mat4 &transform) const {
  drawNode(root, shader, transform);
}

void Model::loadModel(const std::filesystem::path &path) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
    path.string(),
    aiProcess_Triangulate | aiProcess_GenSmoothNormals |
      aiProcess_JoinIdenticalVertices
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
  root = processNode(scene->mRootNode, scene);
}

ModelNode Model::processNode(
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

  modelNode.modelTransform =
    transform * Model::toGlmMat4(node->mTransformation);

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    modelNode.children.push_back(
      processNode(node->mChildren[i], scene, modelNode.modelTransform)
    );
  }

  return modelNode;
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

  return Mesh(std::move(vertices), std::move(indices), std::move(material));
}

void Model::drawNode(
  const ModelNode &node, Shader &shader, const glm::mat4 &transform
) const {
  shader.setMat4("model", transform * node.modelTransform);

  for (auto idx : node.meshIndices)
    meshes[idx].draw(shader);

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