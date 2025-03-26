#version 300 es

precision mediump float;

struct Transition
{
  float cutoff;
  vec3 color;
};

out vec4 FragColor;

in vec2 vs_texcoords;

uniform sampler2D gradient;
uniform Transition transition;

void main()
{
  float transit = texture(gradient, vs_texcoords).r;
  if (transit >= transition.cutoff)
  {
    discard;
  }
  
  FragColor = vec4(transition.color, 1.0);
}