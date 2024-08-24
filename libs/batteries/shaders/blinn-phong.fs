#version 300 es

precision mediump float;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
struct Light {
  float brightness;
  vec3 color;
  vec3 position;
};

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;

uniform Material material;
uniform Light light;
uniform vec3 camera_position;

void main()
{
  // Make sure fragment normal is still length 1 after interpolation.
  vec3 normal = normalize(world_normal);

  // The angle the light ray touches the fragment.
  vec3 light_direction = normalize(light.position - world_position);

  // The angle the looks at the fragment.
  vec3 camera_direction = normalize(camera_position - world_position);

  // Exactly halfway between the camera direction and the light direction.
  vec3 halfway_direction = normalize(light_direction + camera_direction);

  // Calculate diffuse lighting
  float diffuse = max(dot(normal, light_direction), 0.0);;

  // Calculate specular lighting
  float specular = 0.0;
  if (diffuse != 0.0)
  {
    specular = pow(max(dot(normal, halfway_direction), 0.0), material.shininess);
  }

  // Calculate final lighting color
  vec3 light_color = (material.ambient + (material.diffuse * diffuse) + (material.specular * specular)) * light.color;
  vec3 object_color = vec3(normal * 0.5 + 0.5);
  vec3 result = object_color * light_color;

  FragColor = vec4(result, 1.0);
}