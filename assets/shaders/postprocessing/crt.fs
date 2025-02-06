#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;
in vec2 TexCoords;

float PI = 3.14159;

uniform sampler2D screen;

vec2 curvature = vec2(3.0);
vec2 screenResolution = vec2(384.0, 224.0);
vec2 scanLineOpacity = vec2(0.3);
float brightness = 4.0;
float vignetteOpacity = 1.0;
float vignetteRoundness = 2.0;

vec2 curveRemapUV(vec2 uv)
{
  uv = uv * 2.0 - 1.0;
  vec2 offset = abs(uv.yx) / vec2(curvature.x, curvature.y);
  uv = uv + uv * offset * offset;
  uv = uv * 0.5 + 0.5;
  return uv;
}

vec3 scanLineIntensity(float uv, float resolution, float opacity)
{
  float intensity = sin(uv * resolution * PI * 2.0);
  intensity = ((0.5 * intensity) + 0.5) * 0.9 + 0.1;
  return vec3(pow(intensity, opacity));
}

vec3 vignetteIntensity(vec2 uv, vec2 resolution, float opacity, float roundness)
{
  float intensity = uv.x * uv.y * (1.0 - uv.x) * (1.0 - uv.y);
  return vec3(clamp(pow((resolution.x / roundness) * intensity, opacity), 0.0, 1.0));
}

void main()
{
  vec2 uv = curveRemapUV(TexCoords);

  if (uv.x < 0.0 || uv.y < 0.0 || uv.x > 1.0 || uv.y > 1.0)
  {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
  else
  {
    vec3 base_color = texture(screen, uv).rgb;
    base_color *= vignetteIntensity(uv, screenResolution, vignetteOpacity, vignetteRoundness);
    base_color *= scanLineIntensity(uv.x, screenResolution.y, scanLineOpacity.x);
    base_color *= scanLineIntensity(uv.y, screenResolution.x, scanLineOpacity.y);
    base_color *= brightness;

    FragColor = vec4(base_color, 1.0);
  }
}