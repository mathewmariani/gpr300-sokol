#version 300 es

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
}