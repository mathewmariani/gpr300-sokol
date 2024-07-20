#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 texcoords;

uniform sampler2D water_texture;

void main()
{
  FragColor = vec4(texture(water_texture, texcoords).rgb, 1.0);
}