#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 texcoords;

// uniforms
uniform sampler2D water_texture;
uniform float lod_bias;

void main()
{
  float lod = textureQueryLod(texcoords, texcoords).y + lod_bias;
  FragColor = textureLod(water_texture, texcoords, lod);
}