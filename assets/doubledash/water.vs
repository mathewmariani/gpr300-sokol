#version 300 es

precision mediump float;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform vec3 camera_pos;

out vec2 vs_texcoords;
out vec3 to_camera;

void main()
{
  vec4 world_position = model * vec4(in_position, 1.0);
  to_camera = camera_pos - world_position.xyz;
  vs_texcoords = in_texcoord;
  gl_Position = view_proj * world_position;
}