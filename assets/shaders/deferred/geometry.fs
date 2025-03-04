#version 300 es

precision mediump float;

layout(location = 0) out vec4 FragAlbedo;
layout(location = 1) out vec4 FragPosition;
layout(location = 2) out vec4 FragNormal;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

void main()
{
    vec3 object_color = vs_normal.xyz * 0.5 + 0.5;
    FragAlbedo = vec4(object_color, 1.0);
    FragPosition = vec4(vs_position.rgb, 1.0);
    FragNormal = vec4(vs_normal.rgb, 1.0);
}