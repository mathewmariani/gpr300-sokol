#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 texcoords;

// uniforms
uniform sampler2D water_texture;
uniform float lod_bias;
uniform float tiling;
uniform float time;
uniform float top_scale;
uniform float bottom_scale;

float calculateLODBias(vec2 uv)
{
  // Compute derivatives of texture coordinates to estimate LOD
  vec2 dx = dFdx(uv);
  vec2 dy = dFdy(uv);

  // Calculate the rate of change of texture coordinates
  // Use the max derivative length
  float lod = max(length(dx), length(dy));

  // Apply log2 and add a negative bias to switch to lower detail mipmaps faster
  lod = log2(lod) + lod_bias;

  return lod;
}

void main()
{
  float lod = calculateLODBias(texcoords);

  vec2 uv = texcoords * tiling;

  // add a rippling effect by displacing the lookup
  // uv.y += 0.01 * (sin(uv.x * 3.5 + time * 0.35) + sin(uv.x * 4.8 + time * 1.05) + sin(uv.x * 7.3 + time * 0.45)) / 3.0;
  // uv.x += 0.12 * (sin(uv.y * 4.0 + time * 0.5) + sin(uv.y * 6.8 + time * 0.75) + sin(uv.y * 11.3 + time * 0.2)) / 3.0;
  // uv.y += 0.12 * (sin(uv.x * 4.2 + time * 0.64) + sin(uv.x * 6.3 + time * 1.65) + sin(uv.x * 8.2 + time * 0.45)) / 3.0;


  // multiple samplers to add some depth
  vec4 smp1 = textureLod(water_texture, uv + vec2(1.0, 0.0) * sin(time * 0.35), lod);
  vec4 smp2 = textureLod(water_texture, uv + vec2(0.0, 1.0) * sin(time * 0.35), lod);

  FragColor = smp1 + smp2;
}