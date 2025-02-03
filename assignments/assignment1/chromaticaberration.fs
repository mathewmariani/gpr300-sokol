#version 300 es

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
}