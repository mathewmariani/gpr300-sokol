#version 300 es

precision mediump float;

struct Light {
  vec3 color;
  vec3 position;
};

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;
in vec4 vs_light_proj_pos;

// uniforms
uniform sampler2D shadow_map;
uniform sampler2D snow_texture;
uniform Light light;
uniform vec3 camera_position;
uniform float bias;
uniform bool use_pcf;

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

const vec3 top_color = vec3(0.85, 0.95, 1.05);
const vec3 bottom_color = vec3(0.65, 0.80, 0.95);

void main()
{
    float depth = deformationCalculation(vs_light_proj_pos);
    vec3 object_color = texture(snow_texture, vs_texcoord).rgb;
    vec3 snow_color = mix(bottom_color, top_color, depth);

    
    FragColor = vec4(object_color * snow_color, 1.0);
}