#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D heightmap;

void main()
{
  FragColor = vec4(vec3(texture(heightmap, TexCoords).r), 1.0);
}