// clang-format off
#pragma once
const char shapes_vs[] = R"(#version 300 es

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texcoord;
layout (location = 3) in vec4 in_color;

uniform mat4 view_proj;
uniform mat4 model;

void main()
{
  gl_Position = view_proj * model * in_position;
})";
const char shapes_fs[] = R"(#version 300 es

precision mediump float;
precision mediump sampler2DShadow;

out vec4 FragColor;
uniform vec3 color;

void main()
{       
  FragColor = vec4(color, 1.0);
})";
// clang-format on
