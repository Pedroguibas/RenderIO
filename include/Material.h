#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <variant>
#include <memory>

#include "Texture.h"
#include "Shader.h"

struct AlbedoOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 0;
    glm::vec4 baseColor = glm::vec4(1.0f);

    bool mapEnabled = false;
};

struct SpecularOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 1;
    float shininess = 58.0f;
    float intensity = 0.5f;
    glm::vec3 color = glm::vec3(1.0f);
    bool enabled = false;
    bool mapEnabled = false;
};

struct EmissionOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 5;
    float intensity = 1.0f;
    bool enabled = false;
    bool mapEnabled = false;
};

struct MetallicOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 1;
    float value = 0.0f;

    bool mapEnabled = false;
};

struct RoughnessOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 2;
    float value = 0.5f;

    bool mapEnabled = false;
};

struct NormalOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 3;

    bool mapEnabled = false;
};

struct AmbientOcclusionOptions {
    std::shared_ptr<Texture> texture = nullptr;
    unsigned int index = 4;
    float value = 1.0f;

    bool mapEnabled = false;
};

struct PhongShaderOptions {
    AlbedoOptions albedo = {};
    SpecularOptions specular = {};
    EmissionOptions emission = {};
};

struct PBRShaderOptions {
    AlbedoOptions albedo = {};
    EmissionOptions emission = {};
    MetallicOptions metallic = {};
    RoughnessOptions roughness = {};
    NormalOptions normal = {};
    AmbientOcclusionOptions ao = {};
};

struct PhongMaterialShaderStruct {
    string name = "material";
    string albedoRadical = "albedo";
    string specularRadical = "specular";
    string emissionRadical = "emission";
};

struct PBRMaterialShaderStruct {
    string name = "material";
    string albedoRadical = "albedo";
    string emissionRadical = "emission";
    string metallicRadical = "metallic";
    string roughnessRadical = "roughness";
    string normalRadical = "normal";
    string ambientOcclusionRadical = "ao";
};

using ShaderOptions = std::variant<PBRShaderOptions, PhongShaderOptions>;
using ShaderStructOptions =
  std::variant<PBRMaterialShaderStruct, PhongMaterialShaderStruct>;

class Material {
  private:
    ShaderOptions shaderOptions;
    ShaderStructOptions shaderStruct;

    Material(
      const ShaderOptions &shaderOptions, ShaderStructOptions shaderStruct
    );

  public:
    static Material createPBR(
      const PBRShaderOptions &shaderOptions,
      PBRMaterialShaderStruct shaderStruct = {}
    );
    static Material createPhong(
      const PhongShaderOptions &shaderOptions,
      PhongMaterialShaderStruct shaderStruct = {}
    );

    void setShaderStruct(ShaderStructOptions shaderStruct);

    const PBRShaderOptions &getPBRShaderOptions() const;
    PBRShaderOptions &getPBRShaderOptions();

    const PhongShaderOptions &getPhongShaderOptions() const;
    PhongShaderOptions &getPhongShaderOptions();

    void use(Shader &shader) const;
};