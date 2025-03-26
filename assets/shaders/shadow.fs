#version 300 es

precision mediump float;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
struct Ambient {
  float intensity;
  vec3 color;
};
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
uniform Material material;
uniform Ambient ambient;
uniform Light light;
uniform vec3 camera_position;
uniform float bias;
uniform bool use_pcf;

float shadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide, and transform to [0,1] range
    vec3 proj_coords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj_coords = proj_coords * 0.5 + 0.5;

    // get depth values from light's perspective
    float closest_depth = texture(shadow_map, proj_coords.xy).r;
    float current_depth = proj_coords.z;

    // check whether current frag pos is in shadow
    vec3 normal = normalize(vs_normal);
    vec3 light_dir = normalize(light.position - vs_position);
    // float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    float shadow = (current_depth - bias) > closest_depth ? 1.0 : 0.0;

    if (use_pcf)
    {
      shadow = 0.0;
      vec2 texel_size = 1.0 / vec2(1024.0, 1024.0);
      for(int x = -1; x <= 1; ++x)
      {
          for(int y = -1; y <= 1; ++y)
          {
              float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r; 
              shadow += (current_depth - bias) > pcf_depth  ? 1.0 : 0.0;        
          }    
      }
      shadow /= 9.0;
    }

    return shadow;
}


vec3 blinnPhong(vec3 normal, vec3 frag_pos, vec3 light_pos) {
  // normalize inputs
  vec3 view_dir = normalize(camera_position - frag_pos);
  vec3 light_dir = normalize(light_pos - frag_pos);
  vec3 halfway_dir = normalize(light_dir + view_dir);

  // dot products
  float ndotl = max(dot(normal, light_dir), 0.0);
  float ndoth = max(dot(normal, halfway_dir), 0.0);

  // components
  vec3 diffuse = ndotl * material.diffuse;
  vec3 specular = pow(ndoth, material.shininess * 128.0) * material.specular;

  return (diffuse + specular);
}

void main()
{
  vec3 normal = normalize(vs_normal);
  vec3 object_color = (normal * 0.5 + 0.5);

  float shadow = shadowCalculation(vs_light_proj_pos);

  vec3 lighting = blinnPhong(normal, vs_position, light.position);
  lighting *= (1.0 - shadow);
  lighting += ambient.color * material.ambient;
  lighting *= light.color;

  FragColor = vec4(object_color * lighting, 1.0);
}