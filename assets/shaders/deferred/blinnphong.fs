#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 vs_texcoord;

uniform sampler2D g_albedo;
uniform sampler2D g_position;
uniform sampler2D g_normal;

void main()
{
  vec3 color = texture(g_albedo, vs_texcoord).rgb;
  FragColor = vec4(color, 1.0);
}