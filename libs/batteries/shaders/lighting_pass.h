// clang-format off
#pragma once
const char lighting_pass_vs[] = R"(#version 300 es

// attributes
layout(location = 0) in vec2 vPosition;
layout(location = 1) in vec2 vTexCoord;

out vec2 TexCoords;

void main()
{
  TexCoords = vTexCoord;
  gl_Position = vec4(vPosition.xy, 0.0, 1.0);
})";
const char lighting_pass_fs[] = R"(#version 300 es

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
struct ImGUI {
  float radius;
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

uniform ImGUI imgui;

float attenuateExponential(float distance, float radius){
	float i = clamp(1.0 - pow(distance/radius,4.0),0.0,1.0);
	return i * i;
	
}

vec3 calculateLighting(vec3 lightPosition, vec3 lightColor, vec3 normal, vec3 position)
{
	vec3 normalVec = normalize(normal); //Re-normalize normal vector; it may have been distorted when it was interpolated
	vec3 toLight = normalize(lightPosition - position);
	vec3 toCamera = normalize(eye - position);
	
	//Diffuse
	float diffuseFactor = max(dot(normal, toLight), 0.0);

	//Specular
	vec3 h = normalize(toLight + toCamera); //Blinn-Phong uses half angle
	float specularFactor = pow(max(dot(normalVec,h),0.0),material.Shininess);
	
	vec3 totalColor = ((material.Kd * diffuseFactor + material.Ks * specularFactor) * lightColor) + material.Ka * ambient.color;

	return totalColor;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 position){
  vec3 diff = light.position - position;
  vec3 toLight = normalize(diff);
  vec3 lightColor = calculateLighting(light.position, light.color, normal, position);
  float d = length(diff); //Distance to light
  lightColor*=attenuateExponential(d, imgui.radius);
  return lightColor;
}


void main()
{
  vec3 position = texture(g_position, TexCoords).xyz;
  vec3 normal = texture(g_normal, TexCoords).xyz;
  vec3 albedo = texture(g_albedo, TexCoords).rgb;

  vec3 totalLight = vec3(0.0);
  for(int i = 0; i < MAX_LIGHTS; i++)
  {
    totalLight += calcPointLight(lights[i], position, normal);
  }
  FragColor = vec4(albedo * totalLight, 1.0);
})";
// clang-format on
