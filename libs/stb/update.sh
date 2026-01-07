#!/bin/bash
# update the required stb headers

# repo url
STB_PREFIX="https://raw.githubusercontent.com/nothings/stb/master"

# headers
STB_IMAGE_H="$STB_PREFIX/stb_image.h"

# license
STB_LICENSE="$STB_PREFIX/LICENSE"

# output directory
OUTPUT=./libs/stb

# curl all headers
curl $STB_IMAGE_H > $OUTPUT/stb_image.h
curl $STB_LICENSE > $OUTPUT/LICENSE