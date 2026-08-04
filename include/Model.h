#pragma once

#include "Mesh.h"
#include "Shader.h"

#include <vector>
#include <string>

class Model {
  private:
    std::vector<Mesh> meshes;

  public:
    Model(const std::vector<Mesh> &meshes);
    Model(const std::string &path);

    const std::vector<Mesh> &getMeshes() const noexcept;
    std::vector<Mesh> &getMeshes() noexcept;

    void draw(Shader &shader) const;

  private:
    void loadModel(const std::string &path);
    void processNode();
    Mesh processMesh();
};