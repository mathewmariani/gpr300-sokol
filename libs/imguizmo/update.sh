#!/bin/bash
# update the required imguizmo headers

# repo url
IMGUIZMO_PREFIX="https://raw.githubusercontent.com/CedricGuillemet/ImGuizmo/refs/heads/master"

# headers
IMGUIZMO_CPP="$IMGUIZMO_PREFIX/ImGuizmo.cpp"
IMGUIZMO_H="$IMGUIZMO_PREFIX/ImGuizmo.h"

# license
IMGUIZMO_LICENSE="$IMGUIZMO_PREFIX/LICENSE.txt"

# output directory
OUTPUT=./libs/imguizmo

# curl all files
curl $IMGUIZMO_CPP > $OUTPUT/ImGuizmo.cpp
curl $IMGUIZMO_H > $OUTPUT/ImGuizmo.h

# license
curl $IMGUIZMO_LICENSE > $OUTPUT/LICENSE.txt