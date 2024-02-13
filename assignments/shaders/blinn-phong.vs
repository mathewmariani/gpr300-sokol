#version 300 es

// attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec3 vTexCoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

// send to fragment
out vec3 WorldPos;
out vec3 WorldNormal;

void main()
{
  WorldPos = vec3(model * vec4(vPos, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * vNormal;
  gl_Position = view_proj * model * vec4(vPos, 1.0);
}