#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texCoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform mat4 light_view_proj;

out vec3 vs_position;
out vec3 vs_normal;
out vec4 vs_light_proj_pos;

void main()
{
  vs_position = vec3(model * vec4(in_position, 1.0));
  vs_normal = in_normal;

  vs_light_proj_pos = light_view_proj * vec4(vs_position, 1.0);
  gl_Position = view_proj * model * vec4(in_position, 1.0);
}