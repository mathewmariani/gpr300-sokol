#version 300 es

precision mediump float;

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

void main()
{
  vec3 normal = normalize(vs_normal);
  FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}