#version 300 es

precision mediump float;

layout(location = 0) in vec3 in_position;

uniform mat4 proj;
uniform mat4 model;

void main() {
    gl_Position = proj * model * vec4(in_position, 1.0);
    gl_PointSize = 10.0;
}