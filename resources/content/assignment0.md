# Assignment 0
<p class="lead">Blinn-Phong Lighting Model</p>


Checkout the [demo](demo/assignment0.html)


#### Goal

The goal of this assignment is setup and get comfortable with our [development environment][], and refamiliarize ourselves with the graphics pipeline and ImGUI by implementing the [blinn-phong][] reflection model with materials.

<img src="images/phong-reflection-model.png" alt="Fact: This is the primary mode of transportation of the American cowboy." />


#### Instructions

1. Setup Development Environment:
    * Follow the instructions on setting up the [development environment][].


2. Shader Setup:
    * Design a shader program that defines the visual transformations or manipulations you want to apply to the scene. The fragment shader will be responsible for the implemention of the blinn-phong relection model.

<!-- boilerplate code -->
```glsl
#version 300 es

out vec4 FragColor;

in vec3 world_position;
in vec3 world_normal;

void main() {
    vec3 normal = normalize(world_normal);
    vec3 object_color = vec3(normal * 0.5 + 0.5);
    FragColor = vec4(object_color, 1.0);
}
```


3. Lighting Model:
    * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.

<!-- structure for a light -->
```glsl
// An invisible static light source that shines in all directions.
struct Light {
  float brightness;
  vec3 color;
  vec3 position;
};
```

<!-- structure for a material -->
```glsl
struct Material {
  float ambient;
  float diffuse;
  float specular;
  float Shininess;
};
```

<!-- necessary vectors -->
```glsl
// The angle the light ray touches the fragment.
vec3 light_direction = normalize(light.position - FragPos);

// The angle of reflection of the light ray off the surface.
vec3 reflect_direction = reflect(-light_direction, normal);

// The angle the looks at the fragment.
vec3 camera_direction = normalize(camera_position - world_position);

// Exactly halfway between the camera direction and the light direction.
vec3 halfway_direction = normalize(light_direction + camera_direction);
```


3. Diffuse:
    * Diffuse reflectance happens on very rough materials, where imperfections in the surface cause all incoming light rays to bounce in random directions. The brightness is determined solely by the dot product between the direction to the light and surface normal.


4. Specular Reflections:
    * Specular reflections occurs on smooth, mirror-like surfaces. The light rays reflect perfectly across the surface normal and into the viewer’s eye. 


5. Ambient Lighting:
    * In the real world there is a lot of indirect or ambient light, where light bounces off the environment and onto surfaces from all angles. There are many ways to represent this, but the simplest is to just add some amount of constant light color to all parts of the surface.

<!-- lighting calculations -->
```glsl
float ambient = material.ambient * 0.1;
float diffuse = material.diffuse * max(dot(normal, light_direction), 0.0);
float specular = material.specular * pow(max(dot(normal, halfway_direction), 0.0), material.shininess);
```

6. Display the Result:
    * Display the final result on the screen.


### Supplementary Reading

*   [LearnOpenGL - Advanced-Lighting][]


[development environment]: environment.html
[blinn-phong]: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
[LearnOpenGL - Advanced-Lighting]: https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
