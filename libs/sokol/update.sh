#!/bin/bash
# update the required sokol headers

# repo url
SOKOL_PREFIX="https://raw.githubusercontent.com/floooh/sokol/master"

# headers
SOKOL_APP_H="$SOKOL_PREFIX/sokol_app.h"
SOKOL_LOG_H="$SOKOL_PREFIX/sokol_log.h"

# license
SOKOL_LICENSE="$SOKOL_PREFIX/LICENSE"

# output directory
OUTPUT=./libs/sokol

# curl all headers and utils
curl $SOKOL_APP_H > $OUTPUT/sokol_app.h
curl $SOKOL_LOG_H > $OUTPUT/sokol_log.h

# license
curl $SOKOL_LICENSE > $OUTPUT/LICENSE