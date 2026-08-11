#version 460 core

#ifdef HAS_COLOR
in vec4 vColor;
#endif

#ifdef HAS_UV
in vec2 vUv;
#endif

#ifdef HAS_NORMAL
  in vec3 vNormal;
#endif

struct Albedo {
  sampler2D texture;
  vec4 color;
  bool mapEnabled;
};

struct Metallic {
  sampler2D texture;
  float value;
  bool mapEnabled;
};

struct Emission {
  sampler2D texture;
  float intensity;
  bool enabled;
  bool mapEnabled;
};

struct Roughness {
  sampler2D texture;
  float value;
  bool mapEnabled;
}

struct Material {
  Albedo albedo;
  Metallic metallic;
  Roughness roughness;
  Emission emission;
};

struct DirectionLight {
  vec4 color;
  vec3 direction;
};

struct PointLight {
  vec4 color;
  vec3 position;

  float constant;
  float linear;
  float quadratic;
};

struct SpotLight {
  vec4 color;
  vec3 position;
  vec3 direction;

  float constant;
  float linear;
  float quadratic;

  float innerCutoff;
  float outerCutoff;
};

uniform DirectionLight light;
uniform PointLight pLight;
uniform SpotLight sLight;

uniform vec3 cameraPos;
uniform float ambientStrength;
uniform Material material;

in vec3 fragPos;

out vec4 FragColor;

vec3 calcSpecularity(
  vec3 viewDir,
  vec3 dirToLight,
  vec3 normal,
  float reflectivity,
  vec3 lightColor
) {
  if (!material.specular.enabled)
    return vec3(0.0);

  float normalLightDot = dot(normal, dirToLight);

  if (normalLightDot <= 0.0)
      return vec3(0.0);

  vec3 reflectedDir = reflect(-dirToLight, normal);

  float specularStrength = pow(
    max(dot(viewDir, reflectedDir), 0.0),
    material.specular.shininess
  );
  return material.specular.intensity *
    reflectivity *
    specularStrength *
    material.specular.color *
    lightColor;
}

float calcAttenuation(
  vec3 fragmentToLight,
  float constant,
  float linear,
  float quadratic
) {
  float distanceToLight = length(fragmentToLight);

  return 1.0 / (
    constant +
    linear * distanceToLight +
    quadratic * distanceToLight * distanceToLight
  );
}

vec3 calcDirectionLight(
  DirectionLight light,
  vec3 normal,
  vec3 viewDir,
  float reflectivity,
  vec3 baseColor
) {
  vec3 dirToLight = normalize(-light.direction);
  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 ambient = ambientStrength * light.color.rgb;

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = calcSpecularity(viewDir, dirToLight, normal, reflectivity, light.color.rgb);

  return baseColor * (ambient + diffuse) + specular;
}

vec3 calcPointLight(
  PointLight light,
  vec3 normal,
  vec3 viewDir,
  float reflectivity,
  vec3 baseColor
) {
  vec3 fragmentToLight = light.position - fragPos;

  vec3 dirToLight = normalize(fragmentToLight);

  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = calcSpecularity(viewDir, dirToLight, normal, reflectivity, light.color.rgb);

  float attenuation = calcAttenuation(fragmentToLight, light.constant, light.linear, light.quadratic);

  return (baseColor.rgb * diffuse + specular) * attenuation;
}

vec3 calcSpotLight(
  SpotLight light,
  vec3 normal,
  vec3 viewDir,
  float reflectivity,
  vec3 baseColor
) {
  vec3 fragmentToLight = light.position - fragPos;

  vec3 dirToLight = normalize(fragmentToLight);

  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = calcSpecularity(viewDir, dirToLight, normal, reflectivity, light.color.rgb);

  float attenuation = calcAttenuation(fragmentToLight, light.constant, light.linear, light.quadratic);

  vec3 lightToFrag = normalize(fragPos - light.position);

  float theta = dot(
    lightToFrag,
    normalize(light.direction)
  );

  float epsilon = light.innerCutoff - light.outerCutoff;

  float intensity = clamp(
    (theta - light.outerCutoff) / epsilon,
    0.0,
    1.0
  );

  return (baseColor * diffuse + specular) * attenuation * intensity;
}

void main() {
  vec3 baseColor = material.albedo.baseColor;

  vec3 normal = vec3(0.0);
  float reflectivity = 1.0;
  vec3 emission = vec3(0.0);

  baseColor = texture(material.diffuse, vUv);
  if (material.specular.enabled && material.specular.mapEnabled)
    reflectivity = texture(material.specular.texture, vUv).r;

  if (material.emission.mapEnabled && material.emission.enabled)
    emission = texture(material.emission.texture, vUv).rgb * material.emission.intensity;

  baseColor = vColor;

  vec3 finalColor = baseColor.rgb;
  #ifdef HAS_NORMAL  
    normal = vNormal;
    vec3 normalizedNormal = normalize(normal);
    vec3 viewDir = normalize(cameraPos - fragPos);

    finalColor = calcDirectionLight(light, normalizedNormal, viewDir, reflectivity, baseColor.rgb);

    finalColor += calcPointLight(pLight, normalizedNormal, viewDir, reflectivity, baseColor.rgb);

    finalColor += calcSpotLight(sLight, normalizedNormal, viewDir, reflectivity, baseColor.rgb);
  #endif

  FragColor = vec4(finalColor + emission, baseColor.a);
}