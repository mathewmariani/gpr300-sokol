#version 300 es

precision mediump float;

struct Light {
  vec3 color;
  vec3 position;
};

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 model;
uniform mat4 view_proj;
uniform mat4 light_view_proj;

// uniforms
uniform sampler2D shadow_map;
uniform Light light;
uniform vec3 camera_position;
uniform float bias;
uniform bool use_pcf;

out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;
out vec4 vs_light_proj_pos;

float deformationCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide, and transform to [0,1] range
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get depth values from light's perspective
    float depth = texture(shadow_map, proj_coords.xy).r;

    if (use_pcf)
    {
        depth = 0.0;
        vec2 texel_size = 1.0 / vec2(1024.0, 1024.0);
        for(int x = -1; x <= 1; ++x)
        {
            for(int y = -1; y <= 1; ++y)
            {
                float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r; 
                depth += pcf_depth;        
            }    
        }
        depth /= 9.0;
    }

    return depth;
}


void main()
{
    vs_position = vec3(model * vec4(in_position, 1.0));
    vs_normal = in_normal;
    vs_texcoord = in_texcoord;

    vs_light_proj_pos = light_view_proj * vec4(vs_position, 1.0);

    float depth = deformationCalculation(vs_light_proj_pos);
    vs_position.y -= (1.0 - depth);

    gl_Position = view_proj * vec4(vs_position, 1.0);
}