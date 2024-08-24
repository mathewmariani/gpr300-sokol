#version 300 es

precision mediump float;

// attachments
layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;
layout(location = 2) out vec4 frag_albedo;

in vec3 vs_position;
in vec3 vs_normal;

void main()
{
  frag_position = vec4(vs_position, 1.0);
  frag_normal = vec4(normalize(vs_normal), 0.0);
  frag_albedo = vec4(vs_normal.rgb * 0.5 + 0.5, 1.0);
}