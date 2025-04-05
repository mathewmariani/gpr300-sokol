---
title: Assignment 0
description: Blinn-Phong Lighting Model
---

<script src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS-MML_HTMLorMML" type="text/javascript"></script>


#### Goal

The goal of this assignment is setup and get comfortable with our [development environment][], and refamiliarize ourselves with the graphics pipeline and ImGUI by implementing the [blinn-phong][] reflection model with materials.


#### Requirements

1.  Dev environment is set up as expected.
    *   Fork of repository is created and shared with instructor
2.  All steps in the Instructions have been completed.
3.  Version control best practices have been followed
    *   Frequent commits - 1 per meaningful change, bug fix or feature
Descriptive commit messages - actually describe what you changed!
4.  Submit the link to your repository in Github.
5.  Bonus (see bottom of instructions)
    *   Implement normal mapping +10%


#### Instructions

1.  Setup Development Environment:
    *   Follow the instructions on setting up the [development environment][].
2.  Shader Setup:
    *   Design a shader program that defines the visual transformations and manipulations you want to apply to the scene.
    *   The fragment shader will be responsible for the implemention of the blinn-phong relection model.
3.  Lighting Model:
    *   Implement the Blinn-Phong reflectance model, a simple lighting model that allows us to simulate how light reflects off of materials of differing roughnesses. This is achieved by combining different amounts of diffuse and specular reflection.
4.  Material Properties:
    * For a proper implementation of Blinn-Phong, we should expose some material properties (`Kd`, `Ks`, `Ka`, and `alpha`) that will allow us to represent different materials. We can bundle these together into a `uniform struct`. 

<span>
$$
L_o(x, V) = k_a + \sum{(k_d(N \cdot L) + k_s(N \cdot H)^\alpha)}
$$
</span>

<span>
$$
\begin{cases}
k_a = \text{Ambient reflection constant}, \\[1ex]
k_d = \text{Diffuse reflection constant}, \\[1ex]
k_s = \text{Specular reflection constant}, \\[1ex]
\alpha = \text{Shininess constant}, \\[1ex]
L_o = \text{Outgoing light}, \\[1ex]
x = \text{Fragment position}, \\[1ex]
V = \text{View vector}, \\[1ex]
H = \text{Halfway vector}, \\[1ex]
N = \text{Normal vector}, \\[1ex]
L = \text{Light vector}, \\[1ex]
\end{cases}
$$
</span>


<details>
    <summary>/* code spoilers: blinnphong */</summary>

```glsl
// varyings from vertex shader
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

// this won't produce any meaningful results.
// none of these variables have been declared !

vec3 blinnPhong(float NdotL, float NdotH)
{
    vec3 diffuse = Kd * NdotL;
    vec3 specular = Ks * pow(NdotH, alpha);
    return (diffuse + specular);
}

void main()
{
    // TODO: V, H, N, and L still need to be calculated.

    vec3 object_color = vec3(1.0, 0.0, 1.0);
    vec3 lighting = blinnPhong(NdotL, NdotH);
    lighting *= vec3(1.0);  // light color
    lighting += Ka * vec3(1.0); // ambient light

    vec3 outgoing_light = object_color * light_color;
    FragColor = vec4(outgoing_light, 1.0);
}
```
</details>

<details>
    <summary>/* code spoilers: material properties */</summary>

```glsl
struct Material
{
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

// Ka ~> material.ambient
// Kd ~> material.diffuse
// Ks ~> material.specular
// alpha ~> material.shininess;
uniform Material material;
```
</details>


### Supplementary Reading

*   [LearnOpenGL - Basic-Lighting][]
*   [LearnOpenGL - Materials][]
*   [LearnOpenGL - Advanced-Lighting][]
*   [Victor Gordon - Blinn-Phong][]


[development environment]: environment.html
[blinn-phong]: https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_reflection_model
[LearnOpenGL - Basic-Lighting]: https://learnopengl.com/Lighting/Basic-Lighting
[LearnOpenGL - Materials]: https://learnopengl.com/Lighting/Materials
[LearnOpenGL - Advanced-Lighting]: https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
[material properties]: http://devernay.free.fr/cours/opengl/materials.html
[Victor Gordon - Blinn-Phong]: https://youtu.be/-NSBP5q8nNE?si=T6BVsnmdvjJ1pQKN