#version 300 es

// attributes
layout(location = 0) in vec3 pos;

// uniforms
uniform mat4 view_proj;

out vec3 TexCoords;

void main()
{
  TexCoords = pos;
  gl_Position = (view_proj * vec4(pos, 1.0)).xyww;
}