// clang-format off
#pragma once
const char geometry_pass_vs[] = R"(#version 300 es

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

out vec3 vs_position;
out vec3 vs_normal;

void main()
{
  mat4 model = mat4(inst_xxxx, inst_yyyy, inst_zzzz, inst_wwww);
  vec4 position = model * vec4(in_position, 1.0);

  vs_position = position.xyz;
  vs_normal = transpose(inverse(mat3(model))) * in_normal;
  gl_Position = view_proj * position;
})";
const char geometry_pass_fs[] = R"(#version 300 es

precision mediump float;

// attachments
layout(location = 0) out vec4 frag_position;
layout(location = 1) out vec4 frag_normal;
layout(location = 2) out vec4 frag_albedo;

in vec3 vs_position;
in vec3 vs_normal;

void main()
{
  frag_position = vec4(vs_position, 1.0);
  frag_normal = vec4(normalize(vs_normal), 0.0);
  frag_albedo = vec4(vs_normal.rgb * 0.5 + 0.5, 1.0);
})";
// clang-format on
