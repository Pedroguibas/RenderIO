#version 460 core

#ifdef HAS_COLOR
in vec4 vColor;
#endif

in vec2 vUv;
in vec3 vNormal;

const float PI = 3.14159265359;

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
};

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

float calcAttenuation(
  float distanceToLight,
  float constant,
  float linear,
  float quadratic
) {
  return 1.0 / (
    constant +
    linear * distanceToLight +
    quadratic * distanceToLight * distanceToLight
  );
}

float distributionGGX(
  vec3 N,
  vec3 H,
  float roughness
) {
  float a = roughness * roughness;
  float a2 = a * a;

  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float numerator = a2;

  float denominator = NdotH2 * (a2 - 1.0) + 1.0;

  denominator = PI * denominator * denominator;

  return numerator / max(denominator, 0.0001);
}

float geometrySchlickGGX(
  float NdotV,
  float roughness
) {
  float r = roughness + 1.0;
  float k = (r * r) / 8.0;

  float numerator = NdotV;
  float denominator = NdotV * (1.0 - k) + k;

  return numerator / denominator;
}

float gemoetrySmith(
  vec3 N,
  vec3 V,
  vec3 L,
  float roughness
) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);

  float ggxV = geometrySchlickGGX(NdotV, roughness);
  float ggxL = geometrySchlickGGX(NdotL, roughness);

  return ggxV * ggxL;
}

vec3 fresnelSchlick(
  float cosTheta,
  vec3 F0
) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calcPointLight(
  PointLight light,
  vec3 baseColor,
  float metallic,
  float roughness,
  vec3 N,
  vec3 V
) {
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, baseColor, metallic);

  vec3 L = normalize(light.position - fragPos);

  vec3 H = normalize(V + L);

  float distanceToLight = length(light.position - fragPos);

  float attenuation = calcAttenuation(distanceToLight, light.constant, light.linear, light.quadratic);

  float radiance = light.color * attenuation;

  float D = distributionGGX(N, H, roughness);

  float G = gemoetrySmith(N, V, L, roughness);

  vec3 F = fresnelSchlick(
    max(dot(H, V), 0.0),
    F0
  );

  vec3 specular = (D * G * F) / (
    4.0 
    * max(dot(N, V), 0.0)
    * max(dot(N, L), 0.0)
    + 0.0001
  );

  vec3 ks = F;
  vec3 kD = vec3(1.0) - ks;

  float NdotL = max(dot(N, L), 0.0);

  float lo = (
    kD * baseColor / PI + specular
  )
  * radiance
  * NdotL;

  vec3 ambient = ambientStrength * baseColor;

  return ambient + Lo;
}

void maio() {
  vec3 baseColor = material.albedo.color.rgb;
  float metallic = material.metallic.value;
  float roughness = material.roughness.value;

  if (material.albedo.mapEnabled)
    baseColor *= texture(material.albedo.texture, vUv).rgb;

  if (material.metallic.mapEnabled)
    metallic *= texture(material.metallic.texture, vUv).r;

  if (material.roughness.mapEnabled)
    roughness *= texture(material.roughness.texture, vUv).r;

  metallic = clamp(metallic, 0.0, 1.0);
  roughness = clamp(roughness, 0.04, 1.0);

  vec3 N = normalize(vNormal);
  vec3 V = normalize(cameraPos - fragPos);

  baseColor = calcPointLight(pLight, baseColor, metallic, roughness, N, V);

  FragColor = (baseColor, material.albedo.color.a);
}