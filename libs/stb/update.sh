#!/bin/bash
# update the required stb headers

# repo url
SOKOL_PREFIX="https://raw.githubusercontent.com/nothings/stb/master"

# headers
STB_IMAGE="$SOKOL_PREFIX/stb_image.h"

# output directory
OUTPUT=./engine/libs/stb

# curl all headers
curl $STB_IMAGE > $OUTPUT/stb_image.h