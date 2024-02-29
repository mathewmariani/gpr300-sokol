#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec4 inst_xxxx;
layout(location = 4) in vec4 inst_yyyy;
layout(location = 5) in vec4 inst_zzzz;
layout(location = 6) in vec4 inst_wwww;

// uniforms
uniform mat4 view_proj;

out vec4 vs_proj;
out vec4 vs_normal;

void main()
{
  mat4 instance_matrix = mat4(inst_xxxx, inst_yyyy, inst_zzzz, inst_wwww);
  mat4 mvp = view_proj * instance_matrix;
  vs_proj = mvp * vec4(in_position, 1.0);
  vs_normal = vec4(in_normal, 0.0);

  gl_Position = vs_proj;
}