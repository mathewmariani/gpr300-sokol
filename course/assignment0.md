### Assignment 0 - Blinn-Phong Lighting Model

Checkout the [demo](demo/assignment0.html)

#### Goal

The goal of this assignment is setup and get comfortable with our [development environment][], and refamiliarize ourselves with the graphics pipeline and ImGUI by implementing the [blinn-phong][] reflection model.

<img src="images/phong-reflection-model.png" alt="Fact: This is the primary mode of transportation of the American cowboy." />


#### Instructions

1. Setup Development Environment:
    * Follow the instructions on setting up the [development environment][].

2. Shader Setup:
    * Design a shader program that defines the visual transformation or manipulation you want to apply to the scene. The fragment shader will be responsible for the implemention of the blinn-phong relection model.

3. Diffuse:
    * Diffuse reflectance happens on very rough materials, where imperfections in the surface cause all incoming light rays to bounce in random directions. The brightness is determined solely by the dot product between the direction to the light and surface normal.

4. Specular Reflections:
    * Specular reflections occurs on smooth, mirror-like surfaces. The light rays reflect perfectly across the surface normal and into the viewer’s eye. 

5. Ambient Lighting:
    * In the real world there is a lot of indirect or ambient light, where light bounces off the environment and onto surfaces from all angles. There are many ways to represent this, but the simplest is to just add some amount of constant light color to all parts of the surface.

6. Display the Result:
    * Display the final result on the screen.


### Supplementary Reading

*   [LearnOpenGL - Advanced-Lighting][]


[development environment]: environment.html
[blinn-phong]: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
[LearnOpenGL - Advanced-Lighting]: https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
