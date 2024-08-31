#version 300 es

#define MAX_LIGHTS 9

precision mediump sampler2D;
precision mediump float;

struct PointLights {
  vec4 color[64];
  vec4 position[64];
};

struct PointLight {
  vec3 color;
  vec3 position;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 camera_position;
uniform PointLights lights;

PointLight get_light(int i)
{
  PointLight light;
  light.position = lights.position[i].xyz;
  light.color = lights.color[i].rgb;
  return light;
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
  vec3 position = texture(g_position, TexCoords).xyz;
  vec3 normal = texture(g_normal, TexCoords).xyz;
  vec3 albedo = texture(g_albedo, TexCoords).rgb;

  vec3 lighting = vec3(0.0);
  for(int i = 0; i < MAX_LIGHTS; i++)
  {
    PointLight light = get_light(i);
    lighting += blinnPhong(normal, position, light.position, light.color);
  }

  FragColor = vec4(albedo * lighting, 1.0);
}