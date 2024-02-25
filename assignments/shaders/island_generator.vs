#version 300 es

precision mediump float;
precision mediump sampler2D;

// attributes
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform sampler2D heightmap;

struct LandmassProperties {
  float scale;
};
uniform LandmassProperties landmass;

out vec2 TexCoords;

void main()
{
  TexCoords = vTexCoord;

  vec3 pos = vPosition;
  pos.y += landmass.scale * texture(heightmap, TexCoords).r;

  gl_Position = view_proj * model * vec4(pos, 1.0);
}