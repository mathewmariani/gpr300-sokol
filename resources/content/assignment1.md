# Assignment 1
<p class="lead">Framebuffers & Postprocessing</p>


Checkout the [demo](demo/assignment1.html)


#### Goal

The goal of this assignment is to strengthen your understanding of framebuffers by implementing the following post-processing effects.

*   [Box Blur][]
*   [Gaussian Blur][]
*   [Sharpen][]
*   [Edge Detection][]
*   [HDR Tone Mapping][]
*   [Gamma Correction][]
*   [Chromatic Aberration][]
*   [Vignette][]
*   [Lens Distortion][]
*   [Film Grain][]
*   [Screen-Space Fog][]


Build off of the previous assignment to complete this one.


#### Instructions

1. Shader Setup:
    * Design a shader program that defines the visual transformation or manipulation you want to apply to the scene. This shader is responsible for determining the color of each pixel in the output.

2. Framebuffer Setup:
    * Set up a framebuffer object (FBO) to render the scene into a texture instead of the default frame buffer. This texture will serve as the input for the post-processing effect.

3. Render the Scene to the Framebuffer:
    * Render the entire scene to the framebuffer rather than the default frame buffer. This step is crucial for capturing the scene's appearance as a texture.

4. Bind the Default Framebuffer:
    * Switch back to rendering to the default frame buffer for the final display.

5. Activate the Post-Processing Shader:
    * Use the fragment shader created as a post-processing shader. This shader reads the texture rendered to the framebuffer and applies the desired visual effect.

6. Render a Quad:
    * Render a fullscreen quad (two triangles forming a rectangle that covers the entire screen) using the post-processing shader. This quad will be textured with the result from the framebuffer, and the shader will apply the post-processing effect.

7. Display the Result:
    * Display the final result on the screen.


### Supplementary Reading

*   [LearnOpenGL - Framebuffers][]


[LearnOpenGL - Framebuffers]: https://learnopengl.com/Advanced-OpenGL/Framebuffers
[Box Blur]: https://en.wikipedia.org/wiki/Kernel_(image_processing)
[Gaussian Blur]: https://en.wikipedia.org/wiki/Kernel_(image_processing)
[Sharpen]: https://en.wikipedia.org/wiki/Kernel_(image_processing)
[Edge Detection]: https://en.wikipedia.org/wiki/Kernel_(image_processing)
[HDR Tone Mapping]: https://en.wikipedia.org/wiki/Tone_mapping
[Gamma Correction]: https://en.wikipedia.org/wiki/Gamma_correction
[Chromatic Aberration]: https://en.wikipedia.org/wiki/Chromatic_aberration
[Vignette]: https://en.wikipedia.org/wiki/Vignetting
[Lens Distortion]: https://en.wikipedia.org/wiki/Distortion_(optics)
[Film Grain]: https://en.wikipedia.org/wiki/Film_grain
[Screen-space Fog]: https://en.wikipedia.org/wiki/Distance_fog