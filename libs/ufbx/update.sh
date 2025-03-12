#!/bin/bash

# repo url
UFBX_PREFIX="https://raw.githubusercontent.com/ufbx/ufbx/refs/heads/master"

# headers
UFBX_C="$UFBX_PREFIX/ufbx.c"
UFBX_H="$UFBX_PREFIX/ufbx.h"

# license
UFBX_LICENSE="$UFBX_PREFIX/LICENSE"

# output directory
OUTPUT="./libs/ufbx"

# curl all headers and utils
curl $UFBX_C > $OUTPUT/ufbx.c
curl $UFBX_H > $OUTPUT/ufbx.h
curl $UFBX_LICENSE > $OUTPUT/LICENSE