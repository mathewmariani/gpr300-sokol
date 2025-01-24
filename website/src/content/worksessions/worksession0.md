---
title: Work Session 0
description: Physically Based Rendering (PBR)
---

<script src="https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.0/MathJax.js?config=TeX-AMS-MML_HTMLorMML" type="text/javascript"></script>

#### Goal
This worksession involves converting mathematical equations into a shader program for use in the graphics pipeline. You will be provided with equations, and your task is to express them in GLSL (OpenGL Shading Language) code. This exercise aims to develop your skills in translating equations into a fragment shader for visualization on the GPU.

Through this practical application, you'll gain hands-on experience in shader programming, connecting mathematical formulations with visual outputs.


#### Lighting Equation

The lighting equation, sometimes referred to as the "rendering equation", is described to as follows:

<span>
$$
L_o(x, V) = L_e(x, V) + \int{f_r(x, L, V)L_i(x, V)(L \cdot N)}dL
$$
</span>

Since we won't be fully simulating all possible lights within a hemisphere. We're going to simplify the equation to use a fixed number of lights.

<span>
$$
\underbrace{L_o(x, V)}_{\text{outgoing}} = \underbrace{L_e(x, V)}_{\text{emitted}} + \sum{\underbrace{f_r(x, L, V)}_{\text{BDRF}} \underbrace{L_i(x, L)}_{\text{incoming}} \underbrace{(L \cdot N)}_{\text{incoming angle}}}
$$
</span>

##### Bidirectional Reflective Distribution Function (BDRF)

<span>
$$
BRDF = k_d\underbrace{f_{diffuse}}_{\text{lambert}} + k_s\underbrace{f_{specular}}_{\text{cook-torrence}}
\begin{cases}
k_d=1-k_s, \\[2ex]
k_s=F,
\end{cases}
$$
</span>

##### Lambertian Distribution 

<span>
$$
f_{\text{lambert}}=\frac{\text{albedo}}{\pi}
$$
</span>


##### Cook-Torrence Function

<span>
$$
f_{\text{cook-torrence}} = \frac{DGF}{4(V \cdot N)(L \cdot N)}
$$
</span>


##### GGX / Throwbridge-Reitz Normal Distribution Function

<span>
$$
D_{\text{throwbridge-reitz}} = \frac{\alpha^2}{\pi((N \cdot H)^2(\alpha^2 - 1) + 1)^2}
$$
</span>


##### Schlick-Beckmann Geometry Shadowing Function

<span>
$$
G_{\text{schlick-beckmann}} = \frac{(N \cdot X)}{(N \cdot X)(1-k)+k}
\begin{cases}
k=\frac{\alpha}{2}, \\[2ex]
k=\frac{(\text{roughness}+1)^2}{8},
\end{cases}
$$
</span>


##### Smith Model

<span>
$$
G_{\text{smith}} = G_1(L,N)G_1(V,N)
$$
</span>

##### Fresnel-Schlick Function

<span>
$$
F_{\text{fresnel-schlick}} = F_0+(1-F_0)(1 - (V \cdot H))^5
$$
</span>


### Supplementary Reading

*   [LearnOpenGL - PBR][]


[LearnOpenGL - PBR]: https://learnopengl.com/PBR/Theory