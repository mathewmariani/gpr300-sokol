#version 300 es

precision mediump float;
precision mediump int;

struct Light {
  vec3 color;
  vec3 position;
  float radius;
};

// attachments
layout(location = 0) out vec4 frag_lighting;

uniform vec2 textureSize; 
uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform Light light;
uniform vec3 camera_position;

float calculateAttentuation(float dist, float radius)
{
	float i = clamp(1.0 - pow(dist/radius,4.0), 0.0, 1.0);
	return i * i;
}

vec3 blinnPhong(vec3 position, vec3 normal, vec4 material)
{
  vec3 view_dir = normalize(camera_position - position);
  vec3 light_dir = normalize(light.position - position);
  vec3 halfway_dir = normalize(light_dir + view_dir);  

  // dot products
  float ndotl = max(dot(normal, light_dir), 0.0);
  float ndoth = max(dot(normal, halfway_dir), 0.0);

  // components
  vec3 diffuse = ndotl * vec3(material.g);
  vec3 specular = pow(ndoth, material.a * 128.0) * vec3(material.b); 

  float attentuation = calculateAttentuation(length(light.position - position), light.radius);
  return (diffuse + specular) * attentuation * light.color;
}

void main()
{
  vec2 uv = gl_FragCoord.xy / textureSize;

  vec3 position = texture(g_position, uv).xyz;
  vec3 normal = texture(g_normal, uv).xyz;
  vec3 albedo = texture(g_albedo, uv).xyz;
  vec4 material = texture(g_material, uv).rgba;

  vec3 lighting = blinnPhong(position, normal, material);
  frag_lighting = vec4(lighting, 1.0);
}