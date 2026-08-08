#include "Material.h"

#include <exception>

Material::Material(
  const ShaderOptions &shaderOptions, ShaderStructOptions shaderStruct
)
: shaderOptions(shaderOptions) {
  bool isStructPBR =
    std::holds_alternative<PBRMaterialShaderStruct>(shaderStruct);
  bool pbr = std::holds_alternative<PBRShaderOptions>(shaderOptions);

  if ((pbr && !isStructPBR) || (isStructPBR && !pbr))
    throw std::invalid_argument(
      "Both shaderOptions and shaderStructOptions must refer to the same "
      "material type (PBR or Phong)"
    );

  setShaderStruct(shaderStruct);
}

Material Material::createPBR(
  const PBRShaderOptions &shaderOptions, PBRMaterialShaderStruct shaderStruct
) {
  return Material(shaderOptions, std::move(shaderStruct));
}

Material Material::createPhong(
  const PhongShaderOptions &shaderOptions,
  PhongMaterialShaderStruct shaderStruct
) {
  return Material(shaderOptions, std::move(shaderStruct));
}

const PBRShaderOptions &Material::getPBRShaderOptions() const {
  if (std::holds_alternative<PhongShaderOptions>(shaderOptions))
    throw std::logic_error(
      "Material::getPBRShaderOptions can only be called for a PBR "
      "Material"
    );

  return std::get<PBRShaderOptions>(shaderOptions);
}
PBRShaderOptions &Material::getPBRShaderOptions() {
  if (std::holds_alternative<PhongShaderOptions>(shaderOptions))
    throw std::logic_error(
      "Material::getPBRShaderOptions can only be called for a PBR "
      "Material"
    );

  return std::get<PBRShaderOptions>(shaderOptions);
}

const PhongShaderOptions &Material::getPhongShaderOptions() const {
  if (std::holds_alternative<PBRShaderOptions>(shaderOptions))
    throw std::logic_error(
      "Material::getPhongShaderOptions can only be called of a Phong "
      "Material"
    );

  return std::get<PhongShaderOptions>(shaderOptions);
}
PhongShaderOptions &Material::getPhongShaderOptions() {
  if (std::holds_alternative<PBRShaderOptions>(shaderOptions))
    throw std::logic_error(
      "Material::getPhongShaderOptions can only be called of a Phong "
      "Material"
    );

  return std::get<PhongShaderOptions>(shaderOptions);
}

void Material::setShaderStruct(ShaderStructOptions shaderStruct) {
  if (std::holds_alternative<PBRShaderOptions>(shaderOptions)) {

    if (std::holds_alternative<PhongMaterialShaderStruct>(shaderStruct))
      throw std::invalid_argument(
        "A PBR material can only get a PBRMaterialShaderStruct."
      );

    auto structVal = std::get<PBRMaterialShaderStruct>(shaderStruct);

    const string base = structVal.name + '.';

    structVal.albedoRadical =
      base + (structVal.albedoRadical.empty() ? "albedo."
                                              : structVal.albedoRadical + '.');

    structVal.emissionRadical = base + (structVal.emissionRadical.empty()
                                          ? "emission."
                                          : structVal.emissionRadical + '.');

    structVal.metallicRadical = base + (structVal.metallicRadical.empty()
                                          ? "metallic."
                                          : structVal.metallicRadical + '.');

    structVal.normalRadical =
      base + (structVal.normalRadical.empty() ? "normal."
                                              : structVal.normalRadical + '.');

    structVal.roughnessRadical = base + (structVal.roughnessRadical.empty()
                                           ? "roughness."
                                           : structVal.roughnessRadical + '.');

    structVal.ambientOcclusionRadical =
      base + (structVal.ambientOcclusionRadical.empty()
                ? "ambientOcclusion."
                : structVal.ambientOcclusionRadical + '.');

    this->shaderStruct = std::move(structVal);
  } else {

    if (std::holds_alternative<PBRMaterialShaderStruct>(shaderStruct))
      throw std::invalid_argument(
        "A Phong material can only get a PhongMaterialShaderStruct."
      );

    auto structVal = std::get<PhongMaterialShaderStruct>(shaderStruct);

    const string base = structVal.name + '.';

    structVal.albedoRadical =
      base + (structVal.albedoRadical.empty() ? "albedo."
                                              : structVal.albedoRadical + '.');

    structVal.specularRadical = base + (structVal.specularRadical.empty()
                                          ? "specular."
                                          : structVal.specularRadical + '.');

    structVal.emissionRadical = base + (structVal.emissionRadical.empty()
                                          ? "emission."
                                          : structVal.emissionRadical + '.');

    this->shaderStruct = std::move(structVal);
  }
}

void Material::use(Shader &shader) const {

  if (std::holds_alternative<PhongShaderOptions>(shaderOptions)) {
    auto structVal = std::get<PhongMaterialShaderStruct>(shaderStruct);
    auto options = std::get<PhongShaderOptions>(shaderOptions);

    shader.setInt(structVal.albedoRadical + "texture", options.albedo.index);
    shader.setVec3(
      structVal.albedoRadical + "baseColor", options.albedo.baseColor
    );
    shader.setBool(
      structVal.albedoRadical + "mapEnabled", options.albedo.mapEnabled
    );

    shader.setInt(
      structVal.specularRadical + "texture", options.specular.index
    );
    shader.setFloat(
      structVal.specularRadical + "intensity", options.specular.intensity
    );
    shader.setVec3(structVal.specularRadical + "color", options.specular.color);
    shader.setFloat(
      structVal.specularRadical + "shininess", options.specular.shininess
    );
    shader.setBool(
      structVal.specularRadical + "enabled", options.specular.enabled
    );
    shader.setBool(
      structVal.specularRadical + "mapEnabled", options.specular.mapEnabled
    );

    shader.setInt(
      structVal.emissionRadical + "texture", options.emission.index
    );
    shader.setBool(
      structVal.emissionRadical + "enabled", options.emission.enabled
    );
    shader.setBool(
      structVal.emissionRadical + "mapEnabled", options.emission.mapEnabled
    );
    shader.setFloat(
      structVal.emissionRadical + "intensity", options.emission.intensity
    );

  } else {

    auto structVal = std::get<PBRMaterialShaderStruct>(shaderStruct);
    auto options = std::get<PBRShaderOptions>(shaderOptions);

    shader.setInt(structVal.albedoRadical + "texture", options.albedo.index);
    shader.setVec3(
      structVal.albedoRadical + "baseColor", options.albedo.baseColor
    );
    shader.setBool(
      structVal.albedoRadical + "mapEnabled", options.albedo.mapEnabled
    );

    shader.setInt(
      structVal.emissionRadical + "texture", options.emission.index
    );
    shader.setBool(
      structVal.emissionRadical + "enabled", options.emission.enabled
    );
    shader.setBool(
      structVal.emissionRadical + "mapEnabled", options.emission.mapEnabled
    );
    shader.setFloat(
      structVal.emissionRadical + "intensity", options.emission.intensity
    );

    shader.setInt(
      structVal.metallicRadical + "texture", options.metallic.index
    );
    shader.setFloat(
      structVal.metallicRadical + "value", options.metallic.value
    );
    shader.setBool(
      structVal.metallicRadical + "mapEnabled", options.metallic.mapEnabled
    );

    shader.setInt(
      structVal.roughnessRadical + "texture", options.roughness.index
    );
    shader.setFloat(
      structVal.roughnessRadical + "value", options.roughness.value
    );
    shader.setBool(
      structVal.roughnessRadical + "mapEnabled", options.roughness.mapEnabled
    );

    shader.setInt(structVal.normalRadical + "texture", options.normal.index);
    shader.setBool(
      structVal.normalRadical + "mapEnabled", options.normal.mapEnabled
    );

    shader.setInt(
      structVal.ambientOcclusionRadical + "texture", options.ao.index
    );
    shader.setFloat(
      structVal.ambientOcclusionRadical + "value", options.ao.value
    );
    shader.setBool(
      structVal.ambientOcclusionRadical + "mapEnabled", options.ao.mapEnabled
    );
  }
}