#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 texcoords;

uniform sampler2D screen;

void main()
{
  FragColor = vec4(texture(screen, texcoords).rgb, 1.0);
}