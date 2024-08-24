// clang-format off
#pragma once
const char skybox_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec3 pos;

// uniforms
uniform mat4 view_proj;

out vec3 TexCoords;

void main()
{
  TexCoords = pos;
  gl_Position = (view_proj * vec4(pos, 1.0)).xyww;
})";
const char skybox_fs[] = R"(#version 300 es

precision mediump float;
precision mediump samplerCube;

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
  FragColor = texture(skybox, TexCoords);
})";
// clang-format on
