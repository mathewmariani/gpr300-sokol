#version 300 es

#ifdef VERTEX_SHADER

// attributes
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

// uniforms
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos; // Vertex position in world space
out vec3 WorldNormal; // Vertex normal in world space

void main()
{
  WorldPos = vec3(model * vec4(vPos, 1.0));
  WorldNormal = transpose(inverse(mat3(model))) * vNormal;
  gl_Position = projection * view * model * vec4(vPos, 1.0);
}

#endif

#ifdef FRAGMENT_SHADER

precision mediump float;

out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;

uniform vec3 eye;
uniform vec3 ambient;

vec3 _LightDirection = vec3(0.0, -1.0, 0.0);
vec3 _LightColor = vec3(1.0);

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(WorldNormal);

  // Light pointing straight down
  vec3 toLight = -_LightDirection;
  float diffuseFactor = max(dot(normal, toLight), 0.0);

	// Direction towards eye
	vec3 toEye = normalize(eye - WorldPos);

	// Blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal, h), 0.0), 128.0);

	// Combination of specular and diffuse reflection
	vec3 lightColor = (diffuseFactor + specularFactor) * _LightColor;
	vec3 objectColor = vec3(normal * 0.5 + 0.5);

  // Add some ambient light
  lightColor += ambient;

	FragColor = vec4(objectColor * lightColor, 1.0);
}

#endif