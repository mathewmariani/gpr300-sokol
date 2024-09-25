# Work Session 4
<p class="lead">Toon Shading</p>


View [demo](demo/worksession4.html)


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

*   [How The Wind Waker Redefined Cel Shading][]


[How The Wind Waker Redefined Cel Shading]: https://www.youtube.com/watch?v=mnxs6CR6Zrk
