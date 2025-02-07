#version 300 es

precision mediump float;
precision mediump sampler2D;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform vec3 camera_position;

out vec3 to_camera;
out vec2 vs_texcoord;

void main()
{
  vs_texcoord = in_texcoord;

  vec4 world_position = model * vec4(in_position, 1.0);
  to_camera = camera_position - world_position.xyz;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}