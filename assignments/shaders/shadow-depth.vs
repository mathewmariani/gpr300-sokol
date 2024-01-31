#version 300 es

// attributes
layout(location = 0) in vec3 pos;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;

void main()
{
  gl_Position = view_proj * model * vec4(pos, 1.0);
}