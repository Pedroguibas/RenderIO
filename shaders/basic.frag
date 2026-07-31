#version 460 core

#ifdef HAS_COLOR
in vec4 vColor;
#endif

#ifdef HAS_UV
in vec2 vUv;
uniform sampler2D diffuse;
#endif

out vec4 FragColor;

void main() {
  #ifdef HAS_UV
  FragColor = texture(diffuse, vUv);
  #elif defined(HAS_COLOR)
  FragColor = vColor;
  #else
  FragColor = vec4(1.0);
  #endif
}