#version 300 es

precision mediump float;
precision mediump sampler2D;

// attributes
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform vec3 camera_position;

out vec3 to_camera;

void main()
{
  vec4 world_position = model * vec4(vPosition, 1.0);
  to_camera = camera_position - world_position.xyz;

  gl_Position = view_proj * model * vec4(vPosition, 1.0);
}