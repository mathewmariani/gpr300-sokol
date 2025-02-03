#version 300 es

precision mediump float;

// attachments
layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;
layout(location = 2) out vec4 frag_albedo;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

void main()
{
  frag_position = vec4(vs_position.xyz, 1.0);
  frag_normal = vec4(vs_normal.xyz, 1.0);
  frag_albedo = vec4(vs_normal.rgb * 0.5 + 0.5, 1.0);
}