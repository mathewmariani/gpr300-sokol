#version 300 es

// attributes
layout(location = 0) in vec2 pos;

out vec2 uv;

void main()
{
  uv = pos;
  gl_Position = vec4(pos.xy * 2.0 - 1.0, 0.5, 1.0);
}