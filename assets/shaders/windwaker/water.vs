#version 300 es

precision mediump float;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform float scale;
uniform float strength;
uniform float tiling;
uniform float time;
uniform vec3 color;
uniform vec2 direction;

out vec2 texcoords;

float calculateSurface(float x, float z) {
  float y = 0.0;
  y += (sin(x * 1.0 / scale + time * 1.0) + sin(x * 2.3 / scale + time * 1.5) + sin(x * 3.3 / scale + time * 0.4)) / 3.0;
  y += (sin(z * 0.2 / scale + time * 1.8) + sin(z * 1.8 / scale + time * 1.8) + sin(z * 2.8 / scale + time * 0.8)) / 3.0;
  return y;
}

void main()
{
  texcoords = in_texcoord;

  vec3 pos = in_position;
  pos.y += calculateSurface(pos.x, pos.z) * strength;
  pos.y -= calculateSurface(0.0, 0.0) * strength;

  gl_Position = view_proj * model * vec4(pos, 1.0);
}