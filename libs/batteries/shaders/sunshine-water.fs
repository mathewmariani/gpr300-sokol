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
  // Compute derivatives of texture coordinates to estimate LOD
  vec2 dx = dFdx(texcoords);
  vec2 dy = dFdy(texcoords);

  // Calculate the rate of change of texture coordinates
  // Use the max derivative length
  float lod = max(length(dx), length(dy));

  // Apply log2 and add a negative bias to switch to lower detail mipmaps faster
  lod = log2(lod) + lod_bias;
  
  FragColor = textureLod(water_texture, texcoords, lod);
}