#version 300 es

precision mediump float;
precision mediump sampler2DShadow;

out vec4 FragColor;
uniform vec3 light_color;

void main()
{       
  FragColor = vec4(light_color, 1.0);
}