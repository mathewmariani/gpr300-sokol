#version 300 es

precision mediump float;
precision mediump sampler2DShadow;

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

uniform sampler2DShadow shadow_map;

const float spec_power = 2.2;
const float ambient_intensity = 0.25;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadow_map, projCoords);

    // check whether current frag pos is in shadow
    vec3 normal = normalize(WorldNormal);
    vec3 lightDir = normalize(light_pos - WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.05);

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 normal = normalize(WorldNormal);
    vec3 color = vec3(normal * 0.5 + 0.5);

    vec3 lightColor = vec3(1.0);
    
    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    vec3 lightDir = normalize(light_pos - WorldPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(eye_pos - WorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    

    // calculate shadow
    float shadow = ShadowCalculation(light_proj_pos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}