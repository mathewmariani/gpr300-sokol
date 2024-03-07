#version 300 es

precision mediump float;

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

uniform vec3 eye;
uniform Material material;
uniform Ambient ambient;

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(WorldNormal);

  // Light pointing straight down
  float diffuseFactor = max(dot(normal, ambient.direction), 0.0);

  // Direction towards eye
  vec3 to_eye = normalize(eye - WorldPos);

  // Blinn-phong uses half angle
  vec3 h = normalize(ambient.direction + to_eye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

  // Combination of specular and diffuse reflection
  vec3 light_color = (material.Kd * diffuseFactor + material.Ks * specularFactor) * ambient.color;

  // Add some ambient light
  light_color += ambient.color * material.Ka;

  vec3 object_color = vec3(normal * 0.5 + 0.5);
  FragColor = vec4(object_color * light_color, 1.0);
}