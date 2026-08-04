#include "Model.h"

#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <stdexcept>

Model::Model(const std::vector<Mesh> &meshes)
: meshes(std::move(meshes)),
  directory(NULL) {}

Model::Model(const std::filesystem::path &path) : directory(path) {
  loadModel(path);
}

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

void Model::loadModel(const std::filesystem::path &path) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
    path.string(),
    aiProcess_Triangulate | aiProcess_GenSmoothNormals |
      aiProcess_JoinIdenticalVertices
  );

  if (
    scene == nullptr || scene->mRootNode == nullptr ||
    (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE)
  ) {
    throw std::runtime_error(
      "Failed to load model '" + path.string() +
      "': " + importer.GetErrorString()
    );
  }

  directory = path.parent_path();

  meshes.clear();
  processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    const unsigned int meshIndex = node->mMeshes[i];
    const aiMesh *mesh = scene->mMeshes[meshIndex];

    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
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

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    const aiFace &face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  Material material({});

  return Mesh(std::move(vertices), std::move(indices), std::move(material));
}