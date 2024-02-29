#version 300 es

precision mediump sampler2D;
precision mediump float;

struct Ambient {
  vec3 color;
  vec3 direction;
};
struct Material {
  float Ka;
  float Kd;
  float Ks;
  float Shininess;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 eye;
uniform Ambient ambient;
uniform Material material;

void main()
{
  vec3 position = texture(g_position, TexCoords).xyz;
  vec3 normal = texture(g_normal, TexCoords).xyz;
  vec3 albedo = texture(g_albedo, TexCoords).rgb;

  // Make sure fragment normal is still length 1 after interpolation.
  // normal = normalize(normal);

  // Light pointing straight down
  float diffuseFactor = max(dot(normal, ambient.direction), 0.0);

  // Direction towards eye
  vec3 toEye = normalize(eye - position);

  // Blinn-phong uses half angle
  vec3 h = normalize(ambient.direction + toEye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

  // Combination of specular and diffuse reflection
  vec3 lightColor = (material.Kd * diffuseFactor + material.Ks * specularFactor) * vec3(1.0);

  // Add some ambient light
  lightColor += ambient.color * material.Ka;

  FragColor = vec4(albedo * lightColor, 1.0);
}