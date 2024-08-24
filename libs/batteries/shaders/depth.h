// clang-format off
#pragma once
const char depth_vs[] = R"(#version 300 es

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
})";
const char depth_fs[] = R"(#version 300 es

precision mediump float;

out vec4 FragColor;

float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1.0);
})";
// clang-format on
