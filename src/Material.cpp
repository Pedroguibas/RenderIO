#include "Material.h"

Material::Material(const MaterialBuilderOptions &options)
: diffuseIndex(options.diffuseIndex),
  specular(options.specular),
  emission(options.emission) {
  setShaderMaterialUniformOptions(options.shaderStruct);
}

void Material::setDiffuseIndex(unsigned int index) noexcept {
  diffuseIndex = index;
}
unsigned int Material::getDiffuseIndex() const noexcept {
  return diffuseIndex;
}

const SpecularOptions &Material::getSpecularOptions() const noexcept {
  return specular;
}
const EmissionOptions &Material::getEmissionOptions() const noexcept {
  return emission;
}
SpecularOptions &Material::getSpecularOptions() noexcept {
  return specular;
}
EmissionOptions &Material::getEmissionOptions() noexcept {
  return emission;
}

void Material::setSpecularOptions(const SpecularOptions &options) {
  specular = options;
}
void Material::setEmissionOptions(const EmissionOptions &options) {
  emission = options;
}

void Material::setShaderMaterialUniformOptions(
  const ShaderMaterialUniformOptions &shaderStruct
) {
  const string base = shaderStruct.name + '.';

  diffuseBase = base;

  specularBase = base + (shaderStruct.specularRadical.empty()
                           ? "specular."
                           : shaderStruct.specularRadical + '.');

  emissionBase = base + (shaderStruct.emissionRadical.empty()
                           ? "emission."
                           : shaderStruct.emissionRadical + '.');
}

void Material::use(Shader &shader) const {
  shader.setInt(diffuseBase + "diffuse", diffuseIndex);

  if (specular.mapEnabled)
    shader.setInt(specularBase + "texture", specular.index);
  else
    shader.setInt(specularBase + "texture", diffuseIndex);

  shader.setFloat(specularBase + "intensity", specular.intensity);
  shader.setVec3(specularBase + "color", specular.color);
  shader.setFloat(specularBase + "shininess", specular.shininess);
  shader.setBool(specularBase + "enabled", specular.enabled);
  shader.setBool(specularBase + "mapEnabled", specular.mapEnabled);

  if (emission.mapEnabled)
    shader.setInt(emissionBase + "texture", emission.index);
  else
    shader.setInt(emissionBase + "texture", diffuseIndex);

  shader.setBool(emissionBase + "enabled", emission.enabled);
  shader.setBool(emissionBase + "mapEnabled", emission.mapEnabled);
  shader.setFloat(emissionBase + "intensity", emission.intensity);
}