#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;

void main()
{
  vs_position = in_position;
  vs_normal = in_normal;
  vs_texcoord = in_texcoord;

  // fragNormal = (mWorld * vec4(vertNormal, 0.0)).xyz;
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}