#version 300 es

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 vs_texcoords;

void main()
{
  vs_texcoords = in_texcoord;
  gl_Position = vec4(in_position.xy, 0.0, 1.0);
}