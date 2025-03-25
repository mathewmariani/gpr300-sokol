#version 300 es

precision mediump float;

struct Material {
  float ambient;
  float diffuse;
  float specular;
  float shininess;
};

// attachments
layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;
layout(location = 2) out vec4 frag_albedo;
layout(location = 3) out vec4 frag_material;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform Material material;

void main()
{
    vec3 object_color = vs_normal.rgb * 0.5 + 0.5;
    frag_position = vec4(vs_position.xyz, 1.0);
    frag_normal = vec4(vs_normal.xyz, 1.0);
    frag_albedo = vec4(object_color, 1.0);
    frag_material = vec4(material.ambient, material.diffuse, material.specular, material.shininess);
}