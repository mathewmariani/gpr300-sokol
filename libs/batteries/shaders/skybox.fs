#version 300 es

precision mediump float;
precision mediump samplerCube;

out vec4 FragColor;

// uniforms
uniform samplerCube skybox;

// varyings
in vec3 vs_texcoord;

void main()
{
  FragColor = texture(skybox, vs_texcoord);
}