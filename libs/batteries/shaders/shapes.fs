#version 300 es

precision mediump float;
precision mediump sampler2DShadow;

out vec4 FragColor;
uniform vec3 color;

void main()
{       
  FragColor = vec4(color, 1.0);
}