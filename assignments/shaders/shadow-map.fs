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
  float intensity;
  vec3 color;
  vec3 direction;
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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide, and transform to [0,1] range
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get depth values from light's perspective
    float closest_depth = texture(shadow_map, proj_coords);
    float current_depth = proj_coords.z;

    // check whether current frag pos is in shadow
    vec3 normal = normalize(WorldNormal);
    vec3 light_dir = normalize(light_pos - WorldPos);
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

void main()
{
    vec3 normal = normalize(WorldNormal);
    vec3 light_dir = normalize(light_pos - WorldPos);

    // diffuse
    float diff = max(dot(light_dir, normal), 0.0);
    vec3 diffuse = diff * ambient.color;

    // specular
    vec3 to_eye = normalize(eye_pos - WorldPos);
    vec3 h = normalize(light_dir + to_eye);
    float spec = pow(max(dot(normal, h), 0.0), material.Shininess);
    vec3 specular = spec * ambient.color;

    // calculate shadow
    float shadow = ShadowCalculation(light_proj_pos);                   
    vec3 light_color = (ambient.color * material.Ka) + (1.0 - shadow) * (material.Kd * diffuse + material.Ks * specular);
    vec3 object_color = vec3(normal * 0.5 + 0.5);   
    
    FragColor = vec4(light_color * object_color, 1.0);
}