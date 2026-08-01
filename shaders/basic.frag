#version 460 core

#ifdef HAS_COLOR
in vec4 vColor;
#endif

#ifdef HAS_UV
in vec2 vUv;
uniform sampler2D diffuse;
#endif

#ifdef HAS_NORMAL
in vec3 vNormal;
#endif

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
uniform float shininess;

in vec3 fragPos;

out vec4 FragColor;

vec3 calcDirectionLight(
  DirectionLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 baseColor
) {
  vec3 dirToLight = normalize(-light.direction);
  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 ambient = ambientStrength * light.color.rgb;


  vec3 reflectedDir = reflect(-dirToLight, normal);

  float specularStrength = pow(
    max(dot(viewDir, reflectedDir), 0.0),
    shininess
  );

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = specularStrength * light.color.rgb;
  
  return baseColor * (ambient + diffuse) + specular;
}

vec3 calcPointLight(
  PointLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 baseColor
) {
  vec3 fragmentToLight = light.position - fragPos;

  float distanceToLight = length(fragmentToLight);

  vec3 dirToLight = normalize(fragmentToLight);

  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 reflectedDir = reflect(-dirToLight, normal);

  float specularStrength = pow(
    max(
      dot(viewDir, reflectedDir),
      0.0
    ),
    shininess
  );

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = specularStrength * light.color.rgb;

  float attenuation = 1.0 / (
    light.constant +
    light.linear * distanceToLight +
    light.quadratic * light.quadratic * distanceToLight
  );

  return (baseColor.rgb * diffuse + specular) * attenuation;
}

vec3 calcSpotLight(
  SpotLight light,
  vec3 normal,
  vec3 viewDir,
  vec3 baseColor
) {
  vec3 fragmentToLight = light.position - fragPos;

  float distanceToLight = length(fragmentToLight);

  vec3 dirToLight = normalize(fragmentToLight);

  float diffuseStrength = max(dot(normal, dirToLight), 0.0);

  vec3 reflectedDir = reflect(-dirToLight, normal);

  float specularStrength = pow(
    max(
      dot(viewDir, reflectedDir),
      0.0
    ),
    shininess
  );

  vec3 diffuse = diffuseStrength * light.color.rgb;

  vec3 specular = specularStrength * light.color.rgb;

  float attenuation = 1.0 / (
    light.constant +
    light.linear * distanceToLight +
    light.quadratic * light.quadratic * distanceToLight
  );

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
  vec4 baseColor = vec4(1.0);
  vec3 normal = vec3(0.0);

  #ifdef HAS_UV
  baseColor = texture(diffuse, vUv);
  #elif defined(HAS_COLOR)
  baseColor = vColor;
  #endif

  vec3 finalColor = baseColor.rgb;
  #ifdef HAS_NORMAL  
    normal = vNormal;
    vec3 normalizedNormal = normalize(normal);
    vec3 viewDir = normalize(cameraPos - fragPos);

    finalColor = calcDirectionLight(light, normalizedNormal, viewDir, baseColor.rgb);

    finalColor += calcPointLight(pLight, normalizedNormal, viewDir, baseColor.rgb);

    finalColor += calcSpotLight(sLight, normalizedNormal, viewDir, baseColor.rgb);
  #endif

  FragColor = vec4(finalColor, baseColor.a);
}