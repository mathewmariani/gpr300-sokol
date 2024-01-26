#version 300 es

// attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos; // Vertex position in world space
out vec3 WorldNormal; // Vertex normal in world space

void main()
{
  WorldPos = vec3(model * vec4(vPos, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * vNormal;
  gl_Position = projection * view * model * vec4(vPos, 1.0);
}