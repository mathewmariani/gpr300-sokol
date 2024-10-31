// clang-format off
#pragma once
const char skybox_vs[] = R"(#version 300 es

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
})";
const char skybox_fs[] = R"(#version 300 es

precision mediump float;
precision mediump samplerCube;

out vec4 FragColor;

// uniforms
uniform samplerCube skybox;

// varyings
in vec3 vs_texcoord;

void main()
{
  FragColor = texture(skybox, vs_texcoord);
})";
// clang-format on
