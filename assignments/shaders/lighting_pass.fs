#version 300 es

#define MAX_LIGHTS 64

precision mediump sampler2D;
precision mediump float;

struct Ambient {
  vec3 color;
  vec3 direction;
};
struct PointLight {
  float radius;
  vec3 color;
  vec3 position;
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
uniform PointLight lights[MAX_LIGHTS];
uniform Material material;

// linear falloff
float attenuateLinear(float distance, float radius)
{
	return clamp((radius - distance) / radius, 0.0, 1.0);
}

// exponential falloff
float attenuateExponential(float distance, float radius)
{
	float i = clamp(1.0 - pow(distance / radius, 4.0), 0.0, 1.0);
	return i * i;	
}

vec3 calculateDirectionalLight(vec3 position, vec3 normal)
{
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
  return ambient.color * material.Ka;
}

vec3 calculatePointLight(PointLight light, vec3 position, vec3 normal)
{
  vec3 diff = light.position - position;
	// direction toward light position
	vec3 toLight = normalize(diff);
  vec3 toEye = normalize(eye - position);
	
  // Light pointing straight down
  float diffuseFactor = max(dot(normal, toLight), 0.0);  

  // Blinn-phong uses half angle
  vec3 h = normalize(toLight + toEye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

  vec3 lightColor = (diffuseFactor + specularFactor) * light.color;
	
  // attenuation based on distance to light 
	float d = length(diff); 
	lightColor *= attenuateLinear(d, light.radius);
	
  return lightColor;
}

void main()
{
  vec3 position = texture(g_position, TexCoords).xyz;
  vec3 normal = texture(g_normal, TexCoords).xyz;
  vec3 albedo = texture(g_albedo, TexCoords).rgb;

  // vec3 light_color = calculateDirectionalLight(position, normal);
  // for (int i = 0; i < MAX_LIGHTS; i++)
  // {
  //   light_color += calculatePointLight(lights[i], position, normal);
  // }

  // then calculate lighting as usual
  vec3 lighting = albedo * 0.1; // hard-coded ambient component
  vec3 viewDir = normalize(eye - position);
  for(int i = 0; i < MAX_LIGHTS; ++i)
  {
      // diffuse
      vec3 lightDir = normalize(lights[i].position - position);
      vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].color;
      lighting += diffuse;
  }

  FragColor = vec4(lighting, 1.0);
}