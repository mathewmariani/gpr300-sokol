#version 300 es

precision mediump float;

out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;

uniform vec3 eye;
uniform vec3 ambient;
uniform vec3 light_dir;
vec3 _LightColor = vec3(1.0);

struct Material {
  float Ka;
  float Kd;
  float Ks;
  float Shininess;
};
uniform Material material;

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(WorldNormal);

  // Light pointing straight down
  float diffuseFactor = max(dot(normal, light_dir), 0.0);

  // Direction towards eye
  vec3 toEye = normalize(eye - WorldPos);

  // Blinn-phong uses half angle
  vec3 h = normalize(light_dir + toEye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

  // Combination of specular and diffuse reflection
  vec3 lightColor = (material.Kd * diffuseFactor + material.Ks * specularFactor) * _LightColor;
  vec3 objectColor = vec3(normal * 0.5 + 0.5);

  // Add some ambient light
  lightColor += ambient * material.Ka;

  FragColor = vec4(objectColor * lightColor, 1.0);
}