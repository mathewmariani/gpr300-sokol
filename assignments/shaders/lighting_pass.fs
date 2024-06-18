#version 300 es

#define MAX_LIGHTS 25

precision mediump sampler2D;
precision mediump float;

struct Ambient {
  vec3 color;
  vec3 direction;
};
struct PointLights {
  vec4 radius[MAX_LIGHTS];
  vec4 color[MAX_LIGHTS];
  vec4 position[MAX_LIGHTS];
};
struct Material {
  float Ka;
  float Kd;
  float Ks;
  float Shininess;
};
struct PointLight {
  float radius;
  vec3 color;
  vec3 position;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 eye;
uniform Ambient ambient;
uniform PointLights lights;
uniform Material material;

PointLight get_point_light(int index)
{
  return PointLight(
    lights.radius[index].x,
    lights.color[index].rgb,
    lights.position[index].xyz
  );
}

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
  normal = normalize(normal);
  float diffuseFactor = max(dot(normal, ambient.direction), 0.0);

  vec3 to_eye = normalize(eye - position);

  vec3 h = normalize(ambient.direction + to_eye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

  vec3 light_color = (material.Kd * diffuseFactor + material.Ks * specularFactor) * ambient.color;

  return light_color + ambient.color * material.Ka;
}

// vec3 calculatePointLight(PointLight light, vec3 position, vec3 normal)
// {
//   // diffuse
//   vec3 diff = light.position - position;
//   vec3 lightDir = normalize(diff);
//   vec3 diffuse = max(dot(normal, lightDir), 0.0) * light.color;

//   // attenuation
//   float d = length(diff);
//   diffuse *= attenuateLinear(d,light.radius);

//   return diffuse;
// }

vec3 calculatePointLight(PointLight light, vec3 position, vec3 normal)
{
	vec3 diff = light.position - position;

	//Direction toward light position
	vec3 toLight = normalize(diff);

  // Light pointing straight down
  float diffuseFactor = max(dot(normal, toLight), 0.0);

  // Direction towards eye
  vec3 toEye = normalize(eye - position);

  // Blinn-phong uses half angle
  vec3 h = normalize(toLight + toEye);
  float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

	vec3 lightColor = (diffuseFactor + specularFactor) * light.color;
	
  //Attenuation
	float d = length(diff); //Distance to light
	lightColor*=attenuateLinear(d,light.radius);
	return lightColor;
}

void main()
{
  vec3 FragPos = texture(g_position, TexCoords).xyz;
  vec3 Normal = texture(g_normal, TexCoords).xyz;
  vec3 Diffuse = texture(g_albedo, TexCoords).rgb;

  // then calculate lighting as usual
  vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
  vec3 viewDir  = normalize(eye - FragPos);
  for(int i = 0; i < MAX_LIGHTS; ++i)
  {
      PointLight light = get_point_light(i);
      // calculate distance between light source and current fragment
      float distance = length(light.position - FragPos);
      if(distance < light.radius)
      {
          vec3 lightDir = normalize(light.position - FragPos);

          // diffuse
          vec3 diffuse = max(dot(Normal, lightDir), 0.0) * light.color;

          // specular
          vec3 halfwayDir = normalize(lightDir + viewDir);  
          float spec = pow(max(dot(Normal, halfwayDir), 0.0), material.Shininess);
          
          vec3 specular = light.color * spec;

          // attenuation
          // float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
          float attenuation = attenuateLinear(distance, light.radius);
          diffuse *= attenuation;
          specular *= attenuation;
          lighting += diffuse * material.Kd + specular * material.Ks;
      }
  }
  FragColor = vec4(lighting, 1.0);
}

// void main()
// {
//   vec3 position = texture(g_position, TexCoords).xyz;
//   vec3 normal = texture(g_normal, TexCoords).xyz;
//   vec3 albedo = texture(g_albedo, TexCoords).rgb;

//   // vec3 light_color = calculateDirectionalLight(position, normal);  
//   // vec3 light_color = vec3(0.0);
//   // for (int i = 0; i < MAX_LIGHTS; i++)
//   // {
//   //   light_color += calculatePointLight(get_point_light(i), position, normal);
//   // }
//   // FragColor = vec4(light_color * albedo, 1.0);

//   // Make sure fragment normal is still length 1 after interpolation.
//   normal = normalize(normal);

//   // Light pointing straight down
//   float diffuseFactor = max(dot(normal, ambient.direction), 0.0);

//   // Direction towards eye
//   vec3 to_eye = normalize(eye - position);

//   // Blinn-phong uses half angle
//   vec3 h = normalize(ambient.direction + to_eye);
//   float specularFactor = pow(max(dot(normal, h), 0.0), material.Shininess);

//   // Combination of specular and diffuse reflection
//   vec3 light_color = (material.Kd * diffuseFactor + material.Ks * specularFactor) * ambient.color;

//   // Add some ambient light
//   light_color += ambient.color * material.Ka;

//   vec3 object_color = vec3(normal * 0.5 + 0.5);
//   FragColor = vec4(object_color * light_color, 1.0);
// }