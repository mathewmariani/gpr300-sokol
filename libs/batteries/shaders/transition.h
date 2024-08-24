// clang-format off
#pragma once
const char transition_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec2 in_position;
layout(location = 1) in vec2 in_texcoord;

out vec2 texcoords;

void main()
{
  texcoords = in_texcoord;
  gl_Position = vec4(in_position.xy, 0.0, 1.0);
})";
const char transition_fs[] = R"(#version 300 es

precision mediump float;

struct Transition
{
  sampler2D gradient;
  float cutoff;
  vec3 color;
};

out vec4 FragColor;

in vec2 texcoords;

uniform Transition transition;

void main()
{
  float transit = texture(transition.gradient, texcoords).r;
  if (transit >= transition.cutoff) { discard; }
  FragColor = vec4(transition.color, 1.0);
})";
// clang-format on
