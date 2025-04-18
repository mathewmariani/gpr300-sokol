#version 300 es

precision mediump float;
precision mediump sampler2D;

struct Light
{
	vec3 color;
	vec3 position;
};

out vec4 FragColor;

in vec2 vs_texcoord;
in vec4 vs_clipspace;
in vec3 to_camera;
in vec3 to_light;

uniform sampler2D reflectTexture;
uniform sampler2D refractTexture;
uniform sampler2D dudv;
uniform sampler2D normal;

uniform Light light;

uniform float distortion_scale;
uniform float distortion_strength;
uniform float refract_strength;

void main()
{
	vec2 ndc = (vs_clipspace.xy / vs_clipspace.w) / 2.0 + 0.5;
	vec2 reflectCoord = vec2(ndc.x, -ndc.y);
	vec2 refractCoord = vec2(ndc.x, ndc.y);

	// distortion
	vec2 distortion_uv = vs_texcoord * distortion_scale;
	vec2 distortion = texture(dudv, distortion_uv).rg * 2.0 - 1.0; 
	distortion *= distortion_strength;
	reflectCoord += distortion;
	refractCoord += distortion;

	// specular (using normal)
	vec4 normalColor = texture(normal, distortion_uv);
	vec3 n = vec3(normalColor.r * 2.0 - 1.0, normalColor.b, normalColor.g * 2.0 - 1.0);
	n = normalize(n);

    vec3 halfway_dir = normalize(to_light + to_camera);
    float NdotL = max(dot(n, to_light), 0.0);
    float NdotH = max(dot(n, halfway_dir), 0.0);

	vec3 reflectedLight = reflect(normalize(-to_light), n);
	// float specular = max(dot(reflectedLight, to_camera), 0.0);
	float specular = pow(NdotH, 0.25 * 128.0);
	vec3 highlights = light.color * specular * 0.5;

	vec4 reflectColor = texture(reflectTexture, reflectCoord);
	vec4 refractColor = texture(refractTexture, refractCoord);
	float refractiveFactor = dot(normalize(to_camera), vec3(0.0, 1.0, 0.0));
	refractiveFactor = pow(refractiveFactor, refract_strength);


	
	FragColor = mix(reflectColor, refractColor, refractiveFactor);
	FragColor = mix(FragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
	FragColor += vec4(highlights, 0.0);
}