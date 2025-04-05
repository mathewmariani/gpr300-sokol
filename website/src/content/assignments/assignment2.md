---
title: Assignment 2
description: Shadow Mapping
---

#### Goal

The goal of this assignment is to further strengthen your understanding of framebuffers by implementing shadows through the Shadow Mapping technique.

1.  A scene that contains, at minimum:
    *   A 3D Model
2.  Your scene contains a single directional light
I must be able to use ImGUI to easily change its direction. Exposing the direction values directly [-1 to 1] is okay. Don’t forget to normalize!
3.  Scene must contain a ground plane and at least one object casting a shadow onto it.
4.  Shadow mapping is implemented for a single directional light.
5.  A debug view of your shadow map is shown in ImGUI.
6.  Grading for this assignment will be on a tier based system, with each tier having a max grade.
    *   0% - No shadows or shadow map visible
    *   50% - Shadows are not visible, but a debug view of a correct shadow map is provided.
    *   75% - Shadows are visible, but with visible shadow acne and/or peter panning.
    *   90% - Shadows use slope-scale biasing to have no visible shadow acne and minimal peter panning.
        *   Expose min and max bias values in ImGUI
    *   100% - Soft shadows are implemented using Percentage Closer Filtering


Build off of the previous assignments to complete this one.


#### Instructions

1. Begin Here:
    * Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.


### Supplementary Reading

*   [LearnOpenGL - Shadow Mapping][]


[LearnOpenGL - Shadow Mapping]: https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping