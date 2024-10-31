#version 300 es

precision mediump float;
precision mediump sampler2D;

struct Palette {
  vec3 highlight;
  vec3 shadow;
};

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;
in vec2 texcoords;

uniform Palette palette;
uniform sampler2D albedo;

void main()
{
  // calculate the diffuse lighting
  float zatoon = 1.0;

  // apply color palette by mixing
  // between the shadow and highlights
  vec3 light_color = palette.highlight;

  // use the albedo to as the base color
  vec4 object_color = texture(albedo, texcoords).rgba;

  FragColor = object_color * vec4(light_color, 1.0);
}