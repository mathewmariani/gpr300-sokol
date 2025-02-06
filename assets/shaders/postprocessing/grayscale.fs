#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen;

void main()
{
  vec3 color = texture(screen, TexCoords).rgb;
  float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
  FragColor = vec4(average, average, average, 1.0);
}