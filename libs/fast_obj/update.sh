#!/bin/bash

# repo url
FASTOBJ_PREFIX="https://raw.githubusercontent.com/thisistherk/fast_obj/master"

# headers
FASTOBJ_H="$FASTOBJ_PREFIX/fast_obj.h"

# license
FASTOBJ_LICENSE="$FASTOBJ_PREFIX/LICENSE"

# output directory
OUTPUT=./libs/fast_obj

# curl all headers and utils
curl $FASTOBJ_H > $OUTPUT/fast_obj.h
curl $FASTOBJ_LICENSE > $OUTPUT/LICENSE