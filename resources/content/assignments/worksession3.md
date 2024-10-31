# Work Session 3
<p class="lead">Super Mario Sunshine Water</p>

{{{ demo }}}

#### Goal
The goal of this work-session is to explore the power of textures. We will be using a texture as a lookup table to map values from our vertex lighting calculation onto a range authored by an artist.


#### Instructions

1. Vertex Lighting:
    * Apply a simple vertex lighting effect. Because we are trying to achieve a stylized effect we're going to use the diffuse portion of the Phong reflection model and ommit all specular lighting.

2. Toonification:
    * To achieve the cel-shaded effect, we will sample from `ZAtoon.png` which will be used as a lookup map for our vertex lighting values.

3. Color Tint:
    * Using the color palette mix between the highlight and shadow colors to give our object a stylized color tint.


### Supplementary Reading

*   [Deconstructing Super Mario Sunshine][]


[Deconstructing Super Mario Sunshine]: https://blog.mecheye.net/2018/03/deconstructing-the-water-effect-in-super-mario-sunshine/
