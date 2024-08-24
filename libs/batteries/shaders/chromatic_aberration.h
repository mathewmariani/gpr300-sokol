// clang-format off
#pragma once
const char chromatic_aberration_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoord;

out vec2 TexCoords;

void main()
{
  TexCoords = vTexCoord;
  gl_Position = vec4(vPosition.xy, 0.0, 1.0);
})";
const char chromatic_aberration_fs[] = R"(#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen;

const vec3 offset    = vec3(0.009, 0.006, -0.006);
const vec2 direction = vec2(1.0);

void main()
{
  FragColor.r = texture(screen, TexCoords + (direction * vec2(offset.r))).r;
  FragColor.g = texture(screen, TexCoords + (direction * vec2(offset.g))).g;
  FragColor.b = texture(screen, TexCoords + (direction * vec2(offset.b))).b;
})";
// clang-format on
