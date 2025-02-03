#version 300 es

precision highp float;
precision highp sampler2D;
precision highp sampler2DShadow;

struct Ambient {
  float intensity;
  vec3 color;
  vec3 direction;
};
struct Light {
  float brightness;
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
uniform Ambient ambient;
uniform Light light;
uniform vec3 camera_position;

uniform sampler2D albedo;
uniform sampler2DShadow shadow_map;

float shadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide, and transform to [0,1] range
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get depth values from light's perspective
    float closest_depth = texture(shadow_map, proj_coords);
    float current_depth = proj_coords.z;

    // check whether current frag pos is in shadow
    vec3 normal = normalize(vs_normal);
    vec3 light_dir = normalize(light.position - vs_position);
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec3 texel_size = 1.0 / vec3(2048.0, 2048.0, 1.0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcf_depth = texture(shadow_map, proj_coords + vec3(x, y, 0.0) * texel_size); 
            shadow += (current_depth - bias) > pcf_depth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

vec3 blinnPhong(vec3 normal, vec3 frag_pos, vec3 light_pos, vec3 light_color)
{
  vec3 view_dir = normalize(camera_position - frag_pos);
  vec3 light_dir = normalize(light_pos - frag_pos);
  vec3 halfway_dir = normalize(light_dir + view_dir);  

  // diffuse
  float diff = max(dot(light_dir, normal), 0.0);
  vec3 diffuse = diff * light_color;

  // specular
  float spec = pow(max(dot(normal, halfway_dir), 0.0), 64.0);
  vec3 specular = spec * light_color;    

  return diffuse + specular;
}

void main()
{
  vec3 normal = normalize(vs_normal);
  vec3 color = texture(albedo, vs_texcoord).rgb;

  float shadow = shadowCalculation(vs_light_proj_pos);

  vec3 lighting = blinnPhong(normal, vs_position, light.position, light.color);
  lighting *= (1.0 - shadow);
  lighting += (ambient.intensity * ambient.color);

  FragColor = vec4(color * lighting, 1.0);
}