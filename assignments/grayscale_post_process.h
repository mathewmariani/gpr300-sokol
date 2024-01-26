// clang-format off
#pragma once
const char grayscale_post_process_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoord;

out vec2 TexCoords;

void main()
{
  TexCoords = vTexCoord;
  gl_Position = vec4(vPosition.xy, 0.0, 1.0);
})";
const char grayscale_post_process_fs[] = R"(#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen;

void main()
{
  vec3 color = texture(screen, TexCoords).rgb;
  float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  FragColor = vec4(average, average, average, 1.0);
})";
// clang-format on
