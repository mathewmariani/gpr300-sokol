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

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;

void main()
{
  mat4 model = mat4(inst_xxxx, inst_yyyy, inst_zzzz, inst_wwww);
  vec4 position = model * vec4(in_position, 1.0);

  vs_position = position.xyz;
  // vs_normal = transpose(inverse(mat3(model))) * in_normal;
  vs_normal = in_normal;
  vs_texcoord = in_texcoord;
  gl_Position = view_proj * position;
}