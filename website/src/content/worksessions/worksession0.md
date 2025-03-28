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

Since we won't be fully simulating _*all possible lights within a hemisphere*_. We're going to simplify the equation to use a fixed number of lights. I'll give these functions and variables names, so we can more easily reference them later. \( x \) will represent a single fragment within the fragment shader.

<span>
$$
\underbrace{L_o(x, V)}_{\text{outgoing}} = \underbrace{L_e(x, V)}_{\text{emitted}} + \sum{\underbrace{f_r(x, L, V)}_{\text{BDRF}} \underbrace{L_i(x, L)}_{\text{incoming}} \underbrace{(L \cdot N)}_{\text{LdotN}}}
$$
</span>

<details>
    <summary>/* code spoilers */</summary>

```glsl
// cached texture lookups
vec3 alb;   // albedo
float mtl;  // metallic
float rgh;  // roughness
float ao;   // ambient occlusion
float spec; // specular

// cached dot products
float NdotH;
float NdotV;
float NdotL;
float VdotH;
float VdotN;
float LdotN;

// Rendering equation
// N: normal
// V: view vector
// L: light vector
// H: halfway vector
void main()
{
    // pre-sample all textures
    alb = texture(material.albedo, vs_texcoord).rgb;
    mtl = texture(material.metallic, vs_texcoord).r;
    rgh = texture(material.roughness, vs_texcoord).r;
    ao = texture(material.occlusion, vs_texcoord).r;
    spec = texture(material.specular, vs_texcoord).r;

    // normalize all vectors
    vec3 N = normalize(vs_normal);
    vec3 V = normalize(camera_position);
    vec3 L = normalize(light.position);
    vec3 H = normalize(V + L);

    // pre-compute all dot products
    NdotH = max(dot(N, H), 0.0);
    NdotV = max(dot(N, V), 0.0);
    NdotL = max(dot(N, L), 0.0);
    VdotN = max(dot(V, N), 0.0);
    VdotH = max(dot(V, H), 0.0);
    LdotN = max(dot(L, N), 0.0);

    // NOTE: these are just placeholders.
    // It would be appropriate to turn them into functions.
    vec3 emitted = vec3(0.0);
    vec3 brdf = vec3(0.0);
    vec3 incoming = vec3(0.0);
    vec3 pbr = emited + (brdf * incoming * LdotN);

    // NOTE: returning pbr as the final color isn't strictly necessary.
    // At this point, or at any point really, you can styalize how you want.
    FragColor = vec4(pbr, 1.0);
}
```

</details>


##### Bidirectional Reflective Distribution Function (BRDF)

The first variable (or function) were going to tackle is the BRDF. This variable is relatively straight forward as we simply add our diffuse and specular lighting. Notice that \( k_d + k_s = 1 \) this is for energy conservation, no extra light is being produced.

<span>
$$
BRDF = k_d\underbrace{f_{diffuse}}_{\text{lambert}} + k_s\underbrace{f_{specular}}_{\text{cook-torrence}}
\begin{cases}
k_d=1-k_s, \\[2ex]
k_s=F,
\end{cases}
$$
</span>

<details>
    <summary>/* code spoilers */</summary>

```glsl
vec3 BDRF()
{
    // lambertian distribution
    vec3 lambert = alb / PI;

    // NOTE: these are just placeholders.
    // It would be appropriate to turn them into functions.
    vec3 schlickFresnel = vec3(1.0);
    vec3 cookTorrance = vec3(1.0);

    // ratio between reflection and refraction
    // Ks + Kd = 1
    vec3 Ks = schlickFresnel;
    vec3 Kd = vec3(1.0) - Ks;

    // diffuse + specular
    vec3 diffuseBRDF = (Kd * lambert);
    vec3 specularBRDF = (Ks * cookTorrance);
    return diffuseBRDF + specularBRDF;
}
```

</details>


##### Lambertian Distribution 

<span>
$$
f_{\text{lambert}}=\frac{\text{albedo}}{\pi}
$$
</span>


##### Cook-Torrence Function

The Cook-Torrance function simulates light reflection by considering surface roughness, light scattering, and view dependence, producing realistic highlights and material appearance in rendering.

<span>
$$
f_{\text{cook-torrence}} = \frac{DGF}{4(V \cdot N)(L \cdot N)}
$$
</span>

<details>
    <summary>/* code spoilers */</summary>

```glsl
// V: view vector
// H: half-way vector
// L: light vector
// N: normal
// F0: base reflectivity
// alpha: roughness
vec3 cookTorrence(vec3 VdotH, vec3 VdotN, vec3 LdotN, vec3 F0, float alpha)
{
    // D: GGX / Throwbridge-Reitz Normal Distribution Function
    // G: Schlick-Beckmann Geometry Shadowing Function
    // F: Fresnel-Schlick Function
    vec3 cookTorranceNumerator = D(alpha) * G(alpha) * F(F0, VdotH);
    float cookTorranceDenominator = 4.0 * VdotN * LdotN;

    // avoid divide by 0.
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
    return cookTorranceNumerator / cookTorranceDenominator;
}
```

</details>

##### GGX / Throwbridge-Reitz Normal Distribution Function

The GGX/Trowbridge-Reitz NDF models surface microfacet distribution, controlling highlight shape and roughness, making specular reflections appear more realistic in rendering.

<span>
$$
D_{\text{throwbridge-reitz}} = \frac{\alpha^2}{\pi((N \cdot H)^2(\alpha^2 - 1) + 1)^2}
$$
</span>


##### Schlick-Beckmann Geometry Shadowing Function

This approximates light occlusion on rough surfaces, reducing reflection based on viewing angle and surface roughness for realistic shading.

In this case \( x \) will represent a dot product.

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

The Smith model calculates geometric shadowing and masking for microfacet shading, improving realism by accounting for how surface roughness affects light reflection and visibility.

<span>
$$
G_{\text{smith}} = G_1(L,N)G_1(V,N)
$$
</span>


##### Fresnel-Schlick Function

The Fresnel-Schlick function approximates reflectance variation based on the viewing angle, increasing specular reflection at grazing angles for realistic material appearance.

<span>
$$
F_{\text{fresnel-schlick}} = F_0+(1-F_0)(1 - (V \cdot H))^5
$$
</span>


### Supplementary Reading

*   [LearnOpenGL - PBR][]
*   [OGLDEV - PBR][]


[LearnOpenGL - PBR]: https://learnopengl.com/PBR/Theory
[OGLDEV - PBR]: https://www.youtube.com/watch?v=XK_p2MxGBQs&ab_channel=OGLDEV