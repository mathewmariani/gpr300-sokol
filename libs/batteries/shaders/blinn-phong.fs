#version 300 es

precision mediump float;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
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

// uniforms
uniform Material material;
uniform Light light;
uniform vec3 camera_position;

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
  vec3 color = vs_normal * 0.5 + 0.5;

  vec3 lighting = blinnPhong(normal, vs_position, light.position, light.color);
  color *= lighting;

  FragColor = vec4(color, 1.0);
}