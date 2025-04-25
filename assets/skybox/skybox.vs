#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
// layout(location = 1) in vec3 in_normal;
// layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;

out vec3 vs_texcoord;

void main()
{
    vs_texcoord = in_position;
    vec4 position = view_proj * vec4(in_position, 1.0);
    gl_Position = position.xyww;
}