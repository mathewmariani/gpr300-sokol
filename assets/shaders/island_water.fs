#version 300 es

precision mediump float;

out vec4 FragColor;

in vec3 to_camera;

// const vec3 reflect_color = vec3(0.3843, 0.6509, 0.6627);
// const vec3 refract_color = vec3(0.3843, 0.6509, 0.6627);

const vec4 reflect_color = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 refract_color = vec4(0.0, 1.0, 0.0, 0.6);


void main()
{
  float refractiveFactor = dot(normalize(to_camera), vec3(0.0, 1.0, 0.0));

  vec4 water_color = mix(reflect_color, refract_color, refractiveFactor);
  FragColor = vec4(water_color);
}