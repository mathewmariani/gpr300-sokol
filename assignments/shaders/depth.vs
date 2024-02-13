#version 300 es

// attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  gl_Position = projection * view * model * vec4(vPos, 1.0);
}