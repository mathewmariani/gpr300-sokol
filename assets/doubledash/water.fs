#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 vs_texcoords;
in vec3 to_camera;

// uniforms
uniform sampler2D water_spec;
uniform sampler2D water_tex;
uniform sampler2D water_warp;

uniform vec3 color;
uniform float time;
uniform float scale;
uniform float warp_scale;
uniform float spec_scale;
uniform float warp_strength;

uniform float brightness_lower_cutoff;
uniform float brightness_upper_cutoff;

void main()
{
  vec2 uv = vs_texcoords;

  // warp
  vec2 warp_uv = vs_texcoords * warp_scale;
  vec2 warp_scroll = vec2(0.5, 0.5) * time;
  vec2 warp = texture(water_warp, warp_uv + warp_scroll).xy * warp_strength;
  warp = (warp * 2.0) - 1.0;
  
  // albedo
  vec2 albedo_uv = vs_texcoords * scale;
  vec2 albedo_scroll = vec2(-0.5, 0.5) * time;
  vec4 albedo = texture(water_tex, albedo_uv + warp + albedo_scroll);

  vec3 finalColor = color + vec3(albedo.a);

  // specular
  vec2 spec_uv = vs_texcoords * spec_scale;
  vec3 smp1 = texture(water_spec, spec_uv + vec2(1.0, 0.0) * time).rgb;
  vec3 smp2 = texture(water_spec, spec_uv + vec2(1.0, 1.0) * time).rgb;
  vec3 spec = smp1 + smp2;
  
  // blend highlights with fresnel
  float fresnel = dot(normalize(to_camera), vec3(0.0, 1.0, 0.0));
  float brightness = dot(spec, vec3(0.299, 0.587, 0.114));
  if (brightness <= brightness_lower_cutoff || brightness > brightness_upper_cutoff)
  {
    finalColor = mix(finalColor, finalColor + spec, fresnel);
  }

  FragColor = vec4(finalColor, 1.0);
}