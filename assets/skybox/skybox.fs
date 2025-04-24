#version 300 es

precision mediump float;
precision mediump samplerCube;

out vec4 FragColor;

in vec3 vs_texcoord;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, vs_texcoord);
}