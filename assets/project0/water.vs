#version 300 es

precision mediump float;
precision mediump sampler2D;

struct Light
{
	vec3 color;
	vec3 position;
};

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

out vec2 vs_texcoord;
out vec4 vs_clipspace;
out vec3 to_camera;
out vec3 to_light;

uniform mat4 model;
uniform mat4 view_proj;
uniform Light light;
uniform vec3 camera_position;

void main()
{
	vec4 world_position = model * vec4(in_position, 1.0);
	vs_texcoord = in_texcoord;
	vs_clipspace = view_proj * world_position;
	to_camera = camera_position - world_position.xyz;
	to_light = light.position - world_position.xyz;
	gl_Position = vs_clipspace;
}