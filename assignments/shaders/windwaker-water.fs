#version 300 es

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

// constants
const float S1 = 0.90;
const float S2 = 0.02;

void main()
{
  // give direction and motion
  vec2 uv = TexCoords * wave.tiling + vec2(wave.time * wave.direction);

  // add a rippling effect by displacing the lookup
  uv.y += 0.01 * (sin(uv.x * 3.5 + wave.time * 0.35) + sin(uv.x * 4.8 + wave.time * 1.05) + sin(uv.x * 7.3 + wave.time * 0.45)) / 3.0;
  uv.x += 0.12 * (sin(uv.y * 4.0 + wave.time * 0.5) + sin(uv.y * 6.8 + wave.time * 0.75) + sin(uv.y * 11.3 + wave.time * 0.2)) / 3.0;
  uv.y += 0.12 * (sin(uv.x * 4.2 + wave.time * 0.64) + sin(uv.x * 6.3 + wave.time * 1.65) + sin(uv.x * 8.2 + wave.time * 0.45)) / 3.0;

  // multiple samplers to add some depth
  vec4 smp1 = texture(water_texture, uv * 1.0);
  vec4 smp2 = texture(water_texture, uv * 1.0 + vec2(0.2));

  // combine color and sampler values
  FragColor = vec4(wave.color + vec3(smp1.a * S1  - smp2.a * S2), 1.0);
}