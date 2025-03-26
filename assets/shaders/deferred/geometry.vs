#version 300 es

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;

void main()
{
    vec4 world_position = model * vec4(in_position, 1.0);
    vec3 world_normal = transpose(inverse(mat3(model))) * in_normal;

    vs_position = world_position.xyz;
    vs_normal = world_normal;
    vs_texcoord = in_texcoord;

    gl_Position = view_proj * world_position;
}