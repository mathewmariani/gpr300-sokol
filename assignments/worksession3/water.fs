#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 texcoords;

// uniforms
uniform sampler2D texture;
uniform float lod_bias;
uniform float tiling;
uniform float time;
uniform float top_scale;
uniform float bottom_scale;
uniform float brightness_lower_cutoff;
uniform float brightness_upper_cutoff;

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

  // multiple samplers to add some depth
  vec4 smp1 = textureLod(texture, uv + vec2(1.0, 0.0) * (time * 0.35), lod);
  vec4 smp2 = textureLod(texture, uv + vec2(0.0, 1.0) * (time * 0.35), lod);

  vec4 color = smp1 + smp2;

  float brightness = dot(color.rgb, vec3(0.299, 0.587, 0.114)); 
  if (brightness <= brightness_lower_cutoff || brightness > brightness_upper_cutoff)
  {
    discard;
  }

  FragColor = color;
}