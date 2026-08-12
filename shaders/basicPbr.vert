#version 460 core

layout(location = 0) in vec3 aPosition;

#ifdef HAS_COLOR
layout(location = 1) in vec4 aColor;
out vec4 vColor;
#endif

layout(location = 2) in vec2 aUv;
out vec2 vUv;

layout(location = 3) in vec3 aNormal;
out vec3 vNormal;

layout(location = 4) in vec4 aTangent;
out vec4 vTangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

out vec3 fragPos;

void main() {
  vec4 worldPos = model * vec4(aPosition, 1.0);

  gl_Position = perspective * view * worldPos;

  #ifdef HAS_COLOR
  vColor = aColor;
  #endif
  
  vUv = aUv;

  mat3 normalMatrix = transpose(inverse(mat3(model)));

  vec3 N = normalize(normalMatrix * aNormal);

  vec3 T = normalize(normalMatrix * aTangent.xyz);
  T = normalize(T - dot(T, N) * N);

  vTangent = vec4(T, aTangent.w);
  vNormal = N;
  fragPos = worldPos.xyz;
}