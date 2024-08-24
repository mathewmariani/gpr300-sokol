// clang-format off
#pragma once
const char shadow_depth_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec3 position;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;

void main()
{
  gl_Position = view_proj * model * vec4(position, 1.0);
})";
const char shadow_depth_fs[] = R"(#version 300 es

precision mediump float;

void main(){})";
// clang-format on
