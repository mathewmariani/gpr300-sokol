#!/bin/bash
# update tracy

# output directory
OUTPUT=./libs/tracy
pushd $OUTPUT

# download tracy
curl -O -L "https://github.com/wolfpld/tracy/archive/refs/tags/v0.13.1.zip"
unzip v0.13.1.zip

cp -rf tracy-0.13.1/public/* .
# cp -rf tracy/Tracy.hpp .
# cp -rf tracy/TracyC.h .
# cp -rf tracy/TracyOpenGL.hpp .

# rm -rf tracy/
rm -rf tracy-0.13.1
rm -rf v0.13.1.zip

popd