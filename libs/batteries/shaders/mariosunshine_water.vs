#version 300 es

precision mediump float;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;

// uniforms
uniform mat4 model_view_proj;

out vec2 texcoords;

void main()
{
  texcoords = in_texcoord;
  gl_Position = model_view_proj * vec4(in_position, 1.0);
}