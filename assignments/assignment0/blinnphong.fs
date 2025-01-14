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

// uniforms
uniform Material material;
uniform Ambient ambient;
uniform Light light;
uniform vec3 camera_position;

vec3 blinnPhong(vec3 normal, vec3 frag_pos, vec3 light_pos, vec3 light_color) {
  // normalize inputs
  vec3 view_dir = normalize(camera_position - frag_pos);
  vec3 light_dir = normalize(light_pos - frag_pos);
  vec3 halfway_dir = normalize(light_dir + view_dir);  

  // diffuse component
  float ndotl = max(dot(normal, light_dir), 0.0);
  vec3 diffuse = ndotl * light_color * material.diffuse;

  // specular component
  float spec = 0.0;
  if (ndotl > 0.0) {
    spec = pow(max(dot(normal, halfway_dir), 0.0), material.shininess);
  }
  vec3 specular = spec * light_color * material.specular;

  return diffuse + specular;
}

void main()
{
  vec3 normal = normalize(vs_normal);
  vec3 object_color = blinnPhong(normal, vs_position, light.position, light.color);
  object_color += ambient.intensity * ambient.color * material.ambient;
  FragColor = vec4(object_color, 1.0);
}