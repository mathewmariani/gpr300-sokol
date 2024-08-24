// clang-format off
#pragma once
const char island_generator_vs[] = R"(#version 300 es

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

const float offset = 1.0 / 2048.0;
const vec2 offsets[9] = vec2[]( 
  vec2(-offset,  offset), // top-left
  vec2( 0.0,     offset), // top-center
  vec2( offset,  offset), // top-right
  vec2(-offset,  0.0),    // center-left
  vec2( 0.0,     0.0),    // center-center
  vec2( offset,  0.0),    // center-right
  vec2(-offset, -offset), // bottom-left
  vec2( 0.0,    -offset), // bottom-center
  vec2( offset, -offset)  // bottom-right    
);

const float strength = 16.0;
const float kernel[9] = float[](
  1.0, 2.0, 1.0,
  2.0, 4.0, 2.0,
  1.0, 2.0, 1.0 
);

void main()
{
  TexCoords = vTexCoord;

  float height = 0.0;
  for(int i = 0; i < 9; i++)
  {
    float local = texture(heightmap, TexCoords + offsets[i]).r;
    height += local * (kernel[i] / strength);
  }

  vec3 pos = vPosition;
  pos.y += height * landmass.scale;

  gl_Position = view_proj * model * vec4(pos, 1.0);
})";
const char island_generator_fs[] = R"(#version 300 es

precision mediump float;
precision mediump sampler2D;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D heightmap;

const float offset = 1.0 / 2048.0;
const vec2 offsets[9] = vec2[]( 
  vec2(-offset,  offset), // top-left
  vec2( 0.0,     offset), // top-center
  vec2( offset,  offset), // top-right
  vec2(-offset,  0.0),    // center-left
  vec2( 0.0,     0.0),    // center-center
  vec2( offset,  0.0),    // center-right
  vec2(-offset, -offset), // bottom-left
  vec2( 0.0,    -offset), // bottom-center
  vec2( offset, -offset)  // bottom-right    
);

const float strength = 16.0;
const float kernel[9] = float[](
  1.0, 2.0, 1.0,
  2.0, 4.0, 2.0,
  1.0, 2.0, 1.0 
);

void main()
{
  float n = 0.0;
  for(int i = 0; i < 9; i++)
  {
    float local = texture(heightmap, TexCoords + offsets[i]).r;
    n += local * (kernel[i] / strength);
  }

  if(n <= 0.3) {
    FragColor = vec4(0.3843, 0.6509, 0.6627, 1.0);
  } else if(n <= 0.4) {
    FragColor = vec4(0.8392, 0.7137, 0.6196, 1.0);
  } else if(n <= 0.5) {
    FragColor = vec4(0.5961, 0.6784, 0.3529, 1.0);
  } else if(n <= 0.6) {
    FragColor = vec4(0.3961, 0.5216, 0.2549, 1.0);
  } else if(n <= 0.7) {
    FragColor = vec4(0.2784, 0.4627, 0.2706, 1.0);
  } else if(n <= 0.8) {
    FragColor = vec4(0.4275, 0.4627, 0.5294, 1.0);
  } else if(n <= 0.9) {
    FragColor = vec4(0.5176, 0.5529, 0.6039, 1.0);
  } else {
    FragColor = vec4(0.8235, 0.8784, 0.8706, 1.0);
  }
})";
// clang-format on
