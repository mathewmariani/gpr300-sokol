#version 300 es

precision mediump float;
precision mediump int;

const int NR_LIGHTS = 100;

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

in vec2 vs_texcoord;

uniform sampler2D g_albedo;
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform Material material;
uniform Ambient ambient;
uniform Light[NR_LIGHTS] lights;
uniform vec3 camera_position;

vec3 blinnPhong(vec3 position, vec3 normal, Light light)
{
  vec3 view_dir = normalize(camera_position - position);
  vec3 light_dir = normalize(light.position - position);
  vec3 halfway_dir = normalize(light_dir + view_dir);  

  // dot products
  float ndotl = max(dot(normal, light_dir), 0.0);
  float ndoth = max(dot(normal, halfway_dir), 0.0);

  // components
  vec3 diffuse = ndotl * material.diffuse;
  vec3 specular = pow(ndoth, material.shininess * 128.0) * material.specular; 

  return (diffuse + specular) * light.color;
}

void main()
{
  vec3 position = texture(g_position, vs_texcoord).xyz;
  vec3 normal = texture(g_normal, vs_texcoord).xyz;
  vec3 albedo = texture(g_albedo, vs_texcoord).rgb;

  vec3 lighting = vec3(0.0);
  for(int i = 0; i < NR_LIGHTS; ++i)
  {
    lighting += blinnPhong(position, normal, lights[i]);
  }

  lighting += ambient.color * material.ambient;

  FragColor = vec4(albedo * lighting, 1.0);
}