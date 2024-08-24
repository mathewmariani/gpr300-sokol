// clang-format off
#pragma once
const char toon_shadowing_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// send to fragment
out vec3 world_position;
out vec3 world_normal;
out vec2 texcoords;

void main()
{
  texcoords = in_texcoord;
  world_position = vec3(model * vec4(in_position, 1.0));
  world_normal = transpose(inverse(mat3(model))) * in_normal;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
})";
const char toon_shadowing_fs[] = R"(#version 300 es

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
  float intensity;
  vec3 direction;
  vec3 color;
};

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;
in vec2 texcoords;

uniform ToonMaterial material;
uniform Palette palette;
uniform Ambient ambient;

void main()
{
  // calculate the diffuse lighting
  vec3 normal = normalize(world_normal); 
  float diff = (dot(normal, ambient.direction) + 1.0) / 2.0;
  float zatoon = texture(material.zatoon, vec2(diff)).r;

  // apply color palette by mixing between the shadow and highlights
  vec3 light_color = mix(palette.shadow, palette.highlight, zatoon);

  // use the albedo to as the base color
  vec3 object_color = texture(material.albedo, texcoords).rgb;

  FragColor = vec4(object_color * light_color, 1.0);
})";
// clang-format on
