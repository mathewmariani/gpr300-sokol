---
title: Assignment 3
description: Deferred Lighting
---

In this assignment, we will be implementing deferred shading. This is a popular screen-space shading technique that allows many lights to be rendered on the scene without a significant performance hit.

#### Requirements

1.  A scene that contains, at minimum:
    *   A 3D Model
    *   A ground plane
    *   At least 64 point lights visibly shading geometry
    *   Point lights drawn as uniformly colored orbs
2.  Deferred shading is implemented using Blinn-phong reflectance
3.  G-buffer contains the following surface information for use in shading (how you pack your data is up to you!):
    *   Albedo (base texture colors, before lighting)
    *   Positions
    *   Normals
4.  G-buffer debug view in ImGUI. Must show all G-buffer textures.
5.  Bonus:
    *   Use a uniform buffer object to store your point light data. Increase your max number of lights to at least 256.  +5%
    *   Use light volumes for more efficient lighting. See how many lights you can support at interactive frame rates! Your GPU mileage may vary! +5%



### Supplementary Reading

*   [LearnOpenGL - Deferred Shading][]
*   [Valve Deferred Lighting Wiki][]
*   [Deferred Lighting Xbox Tech Demo][]
*   [GTA V - Graphics Study][]


[LearnOpenGL - Deferred Shading]: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
[Valve Deferred Lighting Wiki]: https://developer.valvesoftware.com/wiki/Deferred_lighting
[Deferred Lighting Xbox Tech Demo]: https://www.youtube.com/watch?v=UE4ldJfsDz8&t=2s&ab_channel=dwif87f8dhe44
[GTA V - Graphics Study]: https://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/