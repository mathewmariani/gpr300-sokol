#version 300 es

precision mediump float;

// attachments
layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 frag_brightness;

in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  frag_color = vec4(vs_position.xyz, 1.0);
  frag_brightness = vec4(vs_normal.xyz, 1.0);
}