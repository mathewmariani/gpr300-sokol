#version 300 es

precision mediump float;

// attachments
layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;
layout(location = 2) out vec4 frag_albedo;

in vec4 vs_proj;
in vec4 vs_normal;

void main()
{
  frag_position = vs_proj;
  frag_normal = vs_normal;
  frag_albedo = vec4(1.0, 0.0, 1.0, 1.0);
}