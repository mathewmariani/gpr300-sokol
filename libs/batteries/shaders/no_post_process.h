// clang-format off
#pragma once
const char no_post_process_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 texcoords;

void main()
{
  texcoords = in_texcoord;
  gl_Position = vec4(in_position.xy, 0.0, 1.0);
})";
const char no_post_process_fs[] = R"(#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 texcoords;

uniform sampler2D screen;

void main()
{
  FragColor = vec4(texture(screen, texcoords).rgb, 1.0);
})";
// clang-format on
