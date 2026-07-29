#version 460 core

#ifdef HAS_COLOR
in vec4 vColor;
#endif

#ifdef HAS_UV
in vec2 vUv;
#endif

out vec4 FragColor;

void main() {
  #ifdef HAS_COLOR
  FragColor = vColor;
  #else
  FragColor = vec4(1.0);
  #endif
}