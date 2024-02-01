#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screen;

const float redOffset   =  0.009;
const float greenOffset =  0.006;
const float blueOffset  = -0.006;
const vec2 direction    = vec2(1.0);

void main()
{
  FragColor.r = texture(screen, TexCoords + (direction * vec2(redOffset  ))).r;
  FragColor.g = texture(screen, TexCoords + (direction * vec2(greenOffset))).g;
  FragColor.b = texture(screen, TexCoords + (direction * vec2(blueOffset ))).b;
}