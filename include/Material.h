#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <variant>

#include "Texture.h"
#include "Shader.h"

struct DiffuseOptions {
    unsigned int diffuseIndex = 0;
    glm::vec3 baseColor = glm::vec3(1.0f);

    bool mapEnabled;
};

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

struct MetalicOptions {};

struct RoughnessOptions {};

struct NormalOptions {};

struct AmbientOcclusionOptions {};

struct PhongOptions {
    DiffuseOptions diffuse = {};
    SpecularOptions specular = {};
    EmissionOptions emission = {};
    PhongMaterialShaderStruct shaderStruct = {};
};

struct PBROptions {
    DiffuseOptions diffuse = {};
    EmissionOptions emission = {};
    MetalicOptions metalic = {};
    RoughnessOptions roughness = {};
    NormalOptions normal = {};
    AmbientOcclusionOptions ao = {};
};

struct PhongMaterialShaderStruct {
    string name = "material";
    string diffuseRaical = "diffuse";
    string specularRadical = "specular";
    string emissionRadical = "emission";
};

struct PBRMaterialShaderStruct {
    string name = "material";
    string diffuseRaical = "diffuse";
    string emissionRadical = "emission";
    string metalicRadical = "metalic";
    string roughnessRadical = "roughness";
    string normalRadical = "normal";
    string ambientOcclusionRadical = "ambientOcclusion";
};

using ShaderOptions = std::variant<PBROptions, PhongOptions>;
using ShaderStructOptions =
  std::variant<PBRMaterialShaderStruct, PhongMaterialShaderStruct>;

class Material {
  private:
    bool pbr;
    ShaderOptions options;
    ShaderStructOptions shaderStruct;

    Material(const ShaderOptions &options, ShaderStructOptions shaderStruct);

  public:
    static Material
    createPBR(PBROptions options, PBRMaterialShaderStruct shaderStruct);
    static Material
    createPhong(PhongOptions options, PhongMaterialShaderStruct shaderStruct);

    void setDiffuseIndex(unsigned int index) noexcept;
    unsigned int getDiffuseIndex() const noexcept;

    void setShaderStruct(const ShaderStructOptions &shaderStruct);

    const ShaderOptions &getShaderOptions() const noexcept;
    ShaderOptions &getShaderOptions() noexcept;

    void setShaderOptions(const ShaderOptions &options);

    void use(Shader &shader) const;
};