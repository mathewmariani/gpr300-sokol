#version 300 es

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// varyings
out vec2 vs_texcoord;

void main()
{
  vs_texcoord = in_texcoord;
  gl_Position = vec4(in_position.xy, 0.0, 1.0);
}