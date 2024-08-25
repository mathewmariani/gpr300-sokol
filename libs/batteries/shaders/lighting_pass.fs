#version 300 es

#define MAX_LIGHTS 64

precision mediump sampler2D;
precision mediump float;

struct PointLight {
  vec4 color[64];
  vec4 position[64];
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

in vec2 TexCoords;
out vec4 FragColor;

uniform vec3 eye;
uniform PointLight lights;

void main()
{
  vec3 position = texture(g_position, TexCoords).xyz;
  vec3 normal = texture(g_normal, TexCoords).xyz;
  vec3 albedo = texture(g_albedo, TexCoords).rgb;

  vec3 lighting = vec3(0.0);
  for(int i = 0; i < MAX_LIGHTS; i++)
  {
    float distance = length(vec3(lights.position[i]) - position);
    if(distance < 5.0)
    {
      // diffuse
      vec3 lightDir = normalize(vec3(lights.position[i]) - position);
      vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * vec3(lights.color[i]);
      lighting += diffuse;
    }

  }
  FragColor = vec4(lighting, 1.0);
}