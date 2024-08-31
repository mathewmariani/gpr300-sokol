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
    * The following shader source code snippets implement the “Blinn-Phong reflectance model”, a simple lighting model that allows us to simulate how light reflects off of materials of differing roughnesses. This is achieved by combining different amounts of diffuse and specular reflection.

<!-- structure for a light -->
```glsl
// An invisible static light source that shines in all directions.
struct Light {
  float brightness;
  vec3 color;
  vec3 position;
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

<!-- lighting calculations -->
```glsl
float diffuse = max(dot(normal, light_direction), 0.0);
float specular = pow(max(dot(normal, halfway_direction), 0.0), material.shininess);
```


3. Diffuse:
    * Diffuse reflectance happens on very rough materials, where imperfections in the surface cause all incoming light rays to bounce in random directions. The brightness is determined solely by the dot product between the direction to the light and surface normal.


4. Specular Reflections:
    * Specular reflections occurs on smooth, mirror-like surfaces. The light rays reflect perfectly across the surface normal and into the viewer’s eye. 


5. Ambient Lighting:
    * In the real world there is a lot of indirect or ambient light, where light bounces off the environment and onto surfaces from all angles. There are many ways to represent this, but the simplest is to just add some amount of constant light color to all parts of the surface.


7. Material Properties:
    * So far, for the sake of brevity, I have been using hard-coded values in the shader. For a proper implementation of Blinn-Phong, we should expose properties that allow us to represent different materials.

    We can bundle these together into a uniform struct. Kd and Ks represent the percentage of incoming light that is reflected diffusely vs specularly. To be physically correct, Kd + Ks should not exceed 1. Shininess allows us to change the size of the specular highlight. Shininess is larger for more shiny surfaces, and lower for rougher ones. [material properties][]

<!-- structure for a material -->
```glsl
struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};
```

<!-- material calculations -->
```glsl
// Calculate final lighting color
vec3 light_color = (material.ambient + (material.diffuse * diffuse) + (material.specular * specular)) * light.color;
```


6. Display the Result:
    * Display the final result on the screen.


### Supplementary Reading

*   [LearnOpenGL - Basic-Lighting][]
*   [LearnOpenGL - Materials][]
*   [LearnOpenGL - Advanced-Lighting][]


[development environment]: environment.html
[blinn-phong]: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
[LearnOpenGL - Basic-Lighting]: https://learnopengl.com/Lighting/Basic-Lighting
[LearnOpenGL - Materials]: https://learnopengl.com/Lighting/Materials
[LearnOpenGL - Advanced-Lighting]: https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
[material properties]: http://devernay.free.fr/cours/opengl/materials.html
