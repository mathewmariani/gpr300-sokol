#version 410

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

vec3 effect() {
  return normalize(vs_normal.rgb);
}

void main()
{
  vec3 color = effect();
  FragColor = vec4(color, 1.0);
}