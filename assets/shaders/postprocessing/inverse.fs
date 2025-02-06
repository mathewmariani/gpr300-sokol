#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen;

void main()
{
  vec3 color = 1.0 - texture(screen, TexCoords).rgb;
  FragColor = vec4(color, 1.0);
}