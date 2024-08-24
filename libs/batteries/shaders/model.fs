#version 300 es

precision mediump float;


// uniforms
uniform vec3 eye;
uniform vec3 light_dir;

struct Material {
  sampler2D ao;
  sampler2D col;
  sampler2D mtl;
  sampler2D rgh;
  sampler2D spc;
};
uniform Material material;

out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;
in vec2 TexCoords;

const vec3 _LightColor = vec3(1.0);

void main()
{
  vec3 col = texture(material.col, TexCoords).rgb;
  float ao = texture(material.ao, TexCoords).r;
  float mtl = texture(material.mtl, TexCoords).r;
  float rgh = 1.0 - texture(material.rgh, TexCoords).r;
  float spc = texture(material.spc, TexCoords).r;

  float Ka = rgh;
  float Kd = mtl;
  float Ks = spc;
  float Shininess = rgh * 1024.0;

  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(WorldNormal);

  // Light pointing straight down
  float diffuseFactor = max(dot(normal, light_dir), 0.0);

  // Direction towards eye
  vec3 to_eye = normalize(eye - WorldPos);

  // Blinn-phong uses half angle
  vec3 h = normalize(light_dir + to_eye);
  float specularFactor = pow(max(dot(normal, h), 0.0), Shininess);

  // Combination of specular and diffuse reflection
  vec3 lightColor = (Kd * diffuseFactor + Ks * specularFactor) * _LightColor;
  lightColor += _LightColor * Ka;

  FragColor = vec4(col * ao * lightColor, 1.0);
}