#version 300 es

precision mediump float;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;

out vec2 vs_texcoords;

void main()
{
  vs_texcoords = in_texcoord;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}