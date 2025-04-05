---
layout: "../layouts/Page.astro"
---

# Development Environment
<div class="highlight"></div>

The development environment utilizes [VSCode][], [Cmake][] and the [Emscripten SDK][] to enable an IDE-like workflow that produces a wasm runtime which can be ran in the browser.


### Prerequisites

First make sure that the following tools are in the `$PATH`. To obtain these you can use [Brew][] on macOS, [Scoop][] on Windows, or `apt-get` on Linux.

*   git
*   cmake
*   ninja


### Extensions

You’ll also want the following VSCode extensions:

*   [ms-vscode.cpptools][]
*   [ms-vscode.cmake-tools][]
*   [ms-vscode.wasm-dwarf-debugging][]
*   [ms-vscode.live-server][]


### Getting Started

First youre going to want to clone the repository, and update out submodules.

```sh
git clone https://github.com/mathewmariani/gpr300-sokol
git submodule update --init --recursive
```

We're also going to want to install the Emscripten SDK and add it to the `$PATH`. This will allow us to compile C++ source code to WebAssembly for execution the browser.

```sh
git clone https://github.com/emscripten-core/emsdk
cd emsdk
./emsdk install latest
./emsdk activate --embedded latest
source ./emsdk_env.sh
cd ..
```


### Project Structure

This project was intentionally designed to be **\*small\*** and easily comprehensible. Let's look at some important folders

*   `assignments/` – Each assignment is a self-contained implementation file (.cpp).
*   `build/` - Intermediate files used by cmake, and other build output. 
*   `dist/` - Website build output. 
*   `libs/` - External libraries linked to each assignment.
*   `website/` - Files required to statically generate these webpages.


#### External Libraries

The first thing we should look at are the external libraries, and what they offer

*   `fast_obj` - Fast OBJ file loader.
*   `glm` - Mathematics library for graphics.
*   `imgui` - Bloat-free graphical user interface library.
*   `sokol` - Simple cross-platform libraries for platform abstraction.
*   `stb` - Image loading/decoding from file/memory.

All of these libraries will be used throughout the course.


#### Assignments

Each assignment is an individual implementation file (.cpp) that will need to be added to `/assignmments/CMakeLists.txt` like so

```cmake
add_subdirectory(my_assignment)
```

Behind the scenes a compilation target will be created, and the libraries will automatically be linked and included.

`/assignmments/boilerplate.cpp` is a small implementation file used by all assignments to initialize the platform abstration, and other processes required.

Assets such as models, shaders, and textures **MUST** be placed in `/assets`. This directory will be symlinked to the appropriate directory upon being built.


[VSCode]: https://code.visualstudio.com/
[Cmake]: https://cmake.org/
[Emscripten SDK]: https://emscripten.org/
[ms-vscode.cpptools]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
[ms-vscode.cmake-tools]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools
[ms-vscode.wasm-dwarf-debugging]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.wasm-dwarf-debugging
[ms-vscode.live-server]: https://marketplace.visualstudio.com/items?itemName=ms-vscode.live-server
[Brew]: https://brew.sh
[Scoop]: https://scoop.sh