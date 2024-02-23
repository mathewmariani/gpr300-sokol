#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

out vec4 vs_proj;
out vec4 vs_normal;

void main()
{
  mat4 mvp = view_proj * model;
  vs_proj = mvp * vec4(in_position, 1.0);
  vs_normal = vec4(transpose(inverse(mat3(model))) * in_normal, 1.0);

  gl_Position = vs_proj;
}