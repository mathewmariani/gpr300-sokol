#!/bin/bash
# update the required sokol headers

# repo url
SOKOL_PREFIX="https://raw.githubusercontent.com/floooh/sokol/master"

# headers
SOKOL_APP="$SOKOL_PREFIX/sokol_app.h"
SOKOL_FETCH="$SOKOL_PREFIX/sokol_fetch.h"
SOKOL_GFX="$SOKOL_PREFIX/sokol_gfx.h"
SOKOL_GLUE="$SOKOL_PREFIX/sokol_glue.h"
SOKOL_LOG="$SOKOL_PREFIX/sokol_log.h"
SOKOL_TIME="$SOKOL_PREFIX/sokol_time.h"

# utils
# SOKOL_GL="$SOKOL_PREFIX/util/sokol_gl.h"
SOKOL_IMGUI="$SOKOL_PREFIX/util/sokol_imgui.h"

# output directory
OUTPUT=./libs/sokol

# curl all headers and utils
curl $SOKOL_APP > $OUTPUT/sokol_app.h
curl $SOKOL_FETCH > $OUTPUT/sokol_fetch.h
curl $SOKOL_GFX > $OUTPUT/sokol_gfx.h
curl $SOKOL_GLUE > $OUTPUT/sokol_glue.h
curl $SOKOL_LOG > $OUTPUT/sokol_log.h
curl $SOKOL_TIME > $OUTPUT/sokol_time.h

# utils
curl $SOKOL_IMGUI > $OUTPUT/sokol_imgui.h
