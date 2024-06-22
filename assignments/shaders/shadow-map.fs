#version 300 es

precision highp float;
precision highp sampler2DShadow;

struct Material {
  float Ka;
  float Kd;
  float Ks;
  float Shininess;
};
struct Ambient {
  vec3 direction;
  vec3 color;
};

out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;
in vec4 light_proj_pos;

// uniforms
uniform vec3 light_pos;
uniform vec3 eye_pos;
uniform Material material;
uniform Ambient ambient;

uniform sampler2DShadow shadow_map;

const float spec_power = 2.2;
const float ambient_intensity = 0.25;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide, and transform to [0,1] range
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get closest depth value from light's perspective
    float closest_depth = texture(shadow_map, proj_coords);

    // check whether current frag pos is in shadow
    vec3 normal = normalize(WorldNormal);
    vec3 light_dir = normalize(light_pos - WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);

    // get depth of current fragment from light's perspective
    float current_depth = proj_coords.z;
    float shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

    // // PCF
    // float shadow = 0.0;
    // vec2 texel_size = 1.0 / vec2(1024.0, 1024.0);
    // for(int x = -1; x <= 1; ++x) {
    //     for(int y = -1; y <= 1; ++y) {
    //         float pcf_depth = texture(shadow_map, proj_coords + vec3(x, y, 0.0) * texel_size); 
    //         shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;        
    //     }    
    // }
    // shadow /= 9.0;


    return shadow;
}

void main()
{
    vec3 normal = normalize(WorldNormal);

    vec3 light_color = vec3(1.0);
    vec3 light_dir = normalize(light_pos - WorldPos);

    // ambient
    vec3 ambient = 0.3 * light_color;

    // diffuse
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * light_color;

    // specular
    vec3 view_dir = normalize(eye_pos - WorldPos);
    vec3 h = normalize(light_dir + view_dir);  
    float spec = pow(max(dot(normal, h), 0.0), 64.0);
    vec3 specular = spec * light_color;    

    // calculate shadow
    float shadow = ShadowCalculation(light_proj_pos);                      
    vec3 lighting_color = (ambient + (1.0 - shadow) * (diffuse + specular)); 
    vec3 object_color = vec3(normal * 0.5 + 0.5);   
    
    FragColor = vec4(lighting_color * object_color, 1.0);
}