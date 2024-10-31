#version 300 es

// attributes
layout(location = 0) in vec3 in_position;

// uniforms
uniform mat4 view_proj;

// varyings
out vec3 vs_texcoord;

void main()
{
  vs_texcoord = in_position;
  gl_Position = (view_proj * vec4(in_position, 1.0)).xyww;
}