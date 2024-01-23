### Assignment 1 - Postprocessing


#### Goal

Solidify your understanding of framebuffers by implementing a single pass post processing effect.

View [demo](demo/assignment1.html)


#### Requirements

*   Implement a method to create a custom framebuffer
*     Framebuffer must contain at least one color buffer and one depth buffer.
*   Scene must be rendered to framebuffer
*   Render the framebuffer to the screen as a fullscreen quad or triangle, and apply a post process effect in the fragment shader.
*   All effects must include ImGUI controls for all relevant properties (enabled/disabled, intensity, etc)
*   Some possible effects:
*   Image Convolution
*   Box blur
*   Gaussian blur
*   Sharpen
*   Edge detect
*   HDR tone mapping
*   Gamma correction
*   Chromatic aberration
*   Vignette
*   Lens distortion
*   Film grain
*   Screen-space fog (requires reading from depth buffer)
*   Bonus:
*   Implement ping-pong framebuffers to apply multiple effects, each using their own fragment shader. +10
*   Use version control best practices - descriptive commit messages, frequent commits, no extraneous files.


#### Requirements

OpenGL SuperBible Chapter 9.4 - Offscreen Rendering
[LearnOpenGL - Framebuffers]: https://learnopengl.com/Advanced-OpenGL/Framebuffers