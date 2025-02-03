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
struct Light {
  float brightness;
  vec3 color;
  vec3 position;
};

out vec4 FragColor;

in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoords;

uniform ToonMaterial material;
uniform Palette palette;
uniform Light light;

vec3 toon_lighting(vec3 normal, vec3 frag_pos, vec3 light_pos)
{
  vec3 light_dir = normalize(light_pos - frag_pos);
  float diff = (dot(normal, light_dir) + 1.0) / 2.0;
  float zatoon = texture(material.zatoon, vec2(diff)).r;

  // apply color palette by mixing between the shadow and highlights
  vec3 light_color = mix(palette.shadow, palette.highlight, zatoon);

  return light_color;
}

void main()
{
  vec3 normal = normalize(vs_normal);
  vec3 light_color = toon_lighting(normal, vs_position, light.position);
  vec3 object_color = texture(material.albedo, vs_texcoords).rgb;

  FragColor = vec4(object_color * light_color, 1.0);
}