#!/bin/bash
git clone https://github.com/emscripten-core/emsdk
cd emsdk
./emsdk install latest
./emsdk activate --embedded latest
cd ..

# cmake --preset default -B build
# cmake --build build --preset Debug