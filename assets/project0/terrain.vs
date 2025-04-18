#version 300 es

precision mediump float;
precision mediump sampler2D;

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform sampler2D heightmap;

struct LandmassProperties {
    float scale;
};
uniform LandmassProperties landmass;

out vec3 vs_world_position;
out vec2 vs_texcoord;

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
    vs_texcoord = in_texcoord;

    float height = 0.0;
    for(int i = 0; i < 9; i++)
    {
        float local = texture(heightmap, vs_texcoord + offsets[i]).r;
        height += local * (kernel[i] / strength);
    }

    vec3 pos = in_position;
    pos.y += height * landmass.scale;

    vec4 world_position = model * vec4(pos, 1.0);
    vs_world_position = world_position.xyz;

    gl_Position = view_proj * world_position;
}