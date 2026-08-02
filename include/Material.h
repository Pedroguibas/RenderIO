#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Texture.h"
#include "Shader.h"

struct SpecularOptions {
    unsigned int index = 1;
    float shininess = 58.0f;
    float intensity = 0.5f;
    glm::vec3 color = glm::vec3(1.0f);
    bool enabled = false;
    bool mapEnabled = false;
};

struct EmissionOptions {
    unsigned int index = 2;
    float intensity = 1.0f;
    bool enabled = false;
    bool mapEnabled = false;
};

struct ShaderMaterialUniformOptions {
    string name = "material";
    string specularRadical = "specular";
    string emissionRadical = "emission";
};

struct MaterialBuilderOptions {
    unsigned int diffuseIndex = 0;
    SpecularOptions specular = {};
    EmissionOptions emission = {};
    ShaderMaterialUniformOptions shaderStruct = {};
};

class Material {
  private:
    unsigned int diffuseIndex;
    EmissionOptions emission;
    SpecularOptions specular;

    string diffuseBase;
    string emissionBase;
    string specularBase;

  public:
    Material(const MaterialBuilderOptions &options);

    void setDiffuseIndex(unsigned int index) noexcept;
    unsigned int getDiffuseIndex() const noexcept;

    void setShaderMaterialUniformOptions(
      const ShaderMaterialUniformOptions &shaderStruct
    );

    const SpecularOptions &getSpecularOptions() const noexcept;
    const EmissionOptions &getEmissionOptions() const noexcept;
    SpecularOptions &getSpecularOptions() noexcept;
    EmissionOptions &getEmissionOptions() noexcept;

    void setSpecularOptions(const SpecularOptions &options);
    void setEmissionOptions(const EmissionOptions &options);

    void use(Shader &shader) const;
};