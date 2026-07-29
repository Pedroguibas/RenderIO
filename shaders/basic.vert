#version 460 core

layout(location = 0) in vec3 aPosition;

#ifdef HAS_COLOR
layout(location = 1) in vec4 aColor;
out vec4 vColor;
#endif

#ifdef HAS_UV
layout(location = 2) in vec2 aUv;
out vec2 vUv;
#endif

#ifdef HAS_NORMAL
layout(location = 3) in vec3 aNormal;
out vec3 vNormal;
#endif

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
  gl_Position = perspective * view * model * vec4(aPosition, 1.0);

  #ifdef HAS_COLOR
  vColor = aColor;
  #endif

  #ifdef HAS_UV
  vUv = aUv;
  #endif

  #ifdef HAS_NORMAL
  vNormal = aNormal;
  #endif
}