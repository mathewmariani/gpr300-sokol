#version 300 es

precision mediump float;
precision mediump sampler2D;

struct ToonMaterial {
  sampler2D albedo;
  sampler2D zatoon;
};
struct Palette {
  vec3 highlight;
  vec3 shadow;
};
struct Ambient {
  vec3 direction;
  vec3 color;
};
struct WaterProperties {
  float time;
  vec3 color;
};

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;
in vec2 texcoords;

uniform Palette palette;
uniform Ambient ambient;
uniform WaterProperties water;
uniform sampler2D albedo;

void main()
{
  // calculate the diffuse lighting
  // vec3 normal = normalize(world_normal);
  // float diff = (dot(normal, ambient.direction) + 1.0) / 2.0;
  // float zatoon = texture(material.zatoon, vec2(diff)).r;

  vec2 uv = texcoords + vec2(water.time * vec2(0.0, -1.0));
  uv.y += 0.01 * (sin(uv.x * 3.5 + water.time * 0.35) + sin(uv.x * 4.8 + water.time * 1.05) + sin(uv.x * 7.3 + water.time * 0.45)) / 3.0;
  uv.x += 0.12 * (sin(uv.y * 4.0 + water.time * 0.5) + sin(uv.y * 6.8 + water.time * 0.75) + sin(uv.y * 11.3 + water.time * 0.2)) / 3.0;
  uv.y += 0.12 * (sin(uv.x * 4.2 + water.time * 0.64) + sin(uv.x * 6.3 + water.time * 1.65) + sin(uv.x * 8.2 + water.time * 0.45)) / 3.0;

  // apply color palette by mixing
  // between the shadow and highlights
  // vec3 light_color = mix(palette.shadow, palette.highlight, zatoon);
  vec3 light_color = palette.highlight;

  // use the albedo to as the base color
  vec4 object_color = texture(albedo, uv).rgba + vec4(water.color, 1.0);

  FragColor = object_color * vec4(light_color, 1.0);
}