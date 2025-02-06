#version 300 es

// attributes
layout(location = 0) in vec2 in_postion;
layout(location = 1) in vec2 in_texcoord;

out vec2 vs_texcoord;

void main()
{
  vs_texcoord = in_texcoord;
  gl_Position = vec4(in_postion.xy, 0.0, 1.0);
}