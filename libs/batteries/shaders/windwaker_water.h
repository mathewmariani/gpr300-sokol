// clang-format off
#pragma once
const char windwaker_water_vs[] = R"(#version 300 es

precision mediump float;

// attributes
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;

out vec2 TexCoords;

struct WaveProperties {
  float scale;
  float strength;
  float tiling;
  float time;
  vec3 color;
  vec2 direction;
};
uniform WaveProperties wave;

float calculateSurface(float x, float z) {
  float y = 0.0;
  y += (sin(x * 1.0 / wave.scale + wave.time * 1.0) + sin(x * 2.3 / wave.scale + wave.time * 1.5) + sin(x * 3.3 / wave.scale + wave.time * 0.4)) / 3.0;
  y += (sin(z * 0.2 / wave.scale + wave.time * 1.8) + sin(z * 1.8 / wave.scale + wave.time * 1.8) + sin(z * 2.8 / wave.scale + wave.time * 0.8)) / 3.0;
  return y;
}

void main()
{
  TexCoords = vTexCoord;

  vec3 pos = vPosition;
  pos.y += calculateSurface(pos.x, pos.z) * wave.strength;
  pos.y -= calculateSurface(0.0, 0.0) * wave.strength;

  gl_Position = view_proj * model * vec4(pos, 1.0);
})";
const char windwaker_water_fs[] = R"(#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 TexCoords;

// uniforms
struct WaveProperties {
  float scale;
  float strength;
  float tiling;
  float time;
  vec3 color;
  vec2 direction;
};
uniform WaveProperties wave;
uniform sampler2D water_texture;

uniform float Ts;
uniform float Bs;

void main()
{
  // give direction and motion
  vec2 dir = normalize(wave.direction);
  vec2 uv = TexCoords * wave.tiling + vec2(wave.time * dir);

  // add a rippling effect by displacing the lookup
  uv.y += 0.01 * (sin(uv.x * 3.5 + wave.time * 0.35) + sin(uv.x * 4.8 + wave.time * 1.05) + sin(uv.x * 7.3 + wave.time * 0.45)) / 3.0;
  uv.x += 0.12 * (sin(uv.y * 4.0 + wave.time * 0.5) + sin(uv.y * 6.8 + wave.time * 0.75) + sin(uv.y * 11.3 + wave.time * 0.2)) / 3.0;
  uv.y += 0.12 * (sin(uv.x * 4.2 + wave.time * 0.64) + sin(uv.x * 6.3 + wave.time * 1.65) + sin(uv.x * 8.2 + wave.time * 0.45)) / 3.0;

  // multiple samplers to add some depth
  vec4 smp1 = texture(water_texture, uv * 1.0);
  vec4 smp2 = texture(water_texture, uv * 1.0 + vec2(0.2));

  // combine color and sampler values
  FragColor = vec4(wave.color + vec3(smp1.a * Ts  - smp2.a * Bs), 1.0);
})";
// clang-format on
