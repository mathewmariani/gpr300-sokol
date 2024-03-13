#version 300 es

precision mediump float;

struct Transition
{
  sampler2D gradient;
  float cutoff;
};

out vec4 FragColor;

in vec2 texcoords;

uniform Transition transition;

void main()
{
  float transit = texture(transition.gradient, texcoords).r;
  if (transit >= transition.cutoff) { discard; }
  FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}