# Development Environment

The development environment utilizes [VSCode][], [Cmake][] and the [Emscripten SDK][] to enable an IDE-like workflow that produces a wasm runtime which can be ran in the browser.

### Prerequisites

First make sure that the following tools are in the path:

*   git
*   cmake
*   ninja

### Extensions

You’ll need the following VSCode extensions:

*   [ms-vscode.cpptools][]
*   [ms-vscode.cmake-tools][]
*   [ms-vscode.wasm-dwarf-debugging][]
*   [ms-vscode.live-server][]

### Project Structure

This project was intentionally designed to be **\*small\*** and easily comprehensible. Let's look at some important folders

*   `assignments/` – Each assignment is a self-contained implementation file (.cpp).
*   `build/` - Intermediate files used by cmake, and other build output. 
*   `libs/` - External libraries linked to each assignment.


#### External Libraries

The first thing we should look at are the external libraries, and what they offer

*   `fast_obj` - Fast OBJ file loader.
*   `glm` - Mathematics library for graphics.
*   `imgui` - Bloat-free graphical user interface library.
*   `sokol` - Simple cross-platform libraries for platform abstraction and 3D-API wrapping.
*   `stb` - Image loading/decoding from file/memory.

All of these libraries will be used throughout the course.


#### Assignments

Each assignment is an individual implementation file (.cpp) that will need to be added to `CMakeLists.txt` like so

```cmake
add_assignment(assignment0 assignment0.cpp batteries.h)
```

Behind the scenes a compilation target will be created, and the libraries will automatically be linked and included.

`batteries.h` is a small single-header library of helpful rendering types and functions. Note that these types are simply data-structures and are not meant to create an abstraction layer, but to make common operations simple and easy. You are encouraged to create your own library to add desired functionality.

Assets such as textures, models, and shaders **MUST** be placed in `assignments/assets`. This directory will be automatically copied to the appropriate location upon being built. The assets folder must exist. Do not delete it!


[VSCode]: https://code.visualstudio.com/
[Cmake]: https://cmake.org/
[Emscripten SDK]: https://emscripten.org/
[ms-vscode.cpptools]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
[ms-vscode.cmake-tools]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
[ms-vscode.wasm-dwarf-debugging]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.wasm-dwarf-debugging
[ms-vscode.live-server]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.live-server