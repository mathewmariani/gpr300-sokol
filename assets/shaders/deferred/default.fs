#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D g_albedo;
uniform sampler2D g_lighting;

void main()
{
  vec3 albedo = texture(g_albedo, vs_texcoord).rgb;
  vec3 lighting = texture(g_lighting, vs_texcoord).rgb;
  FragColor = vec4(albedo * lighting, 1.0);
}