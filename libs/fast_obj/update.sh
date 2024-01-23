#!/bin/bash

# repo url
FASTOBJ_PREFIX="https://raw.githubusercontent.com/thisistherk/fast_obj/master"

# headers
FASTOBJ_H="$FASTOBJ_PREFIX/fast_obj.h"

# output directory
OUTPUT=./libs/fast_obj

# curl all headers and utils
curl $FASTOBJ_H > $OUTPUT/fast_obj.h