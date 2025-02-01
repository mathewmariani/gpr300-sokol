#!/bin/bash
# update the required sokol headers

# repo url
IMGUI_PREFIX="https://raw.githubusercontent.com/ocornut/imgui/master"

# headers
IMGUI_CONFIG_H="$IMGUI_PREFIX/imconfig.h"
IMGUI_DRAW_CPP="$IMGUI_PREFIX/imgui_draw.cpp"
IMGUI_INTERNAL_H="$IMGUI_PREFIX/imgui_internal.h"
IMGUI_TABLES_CPP="$IMGUI_PREFIX/imgui_tables.cpp"
IMGUI_WIDGETS_CPP="$IMGUI_PREFIX/imgui_widgets.cpp"
IMGUI_IMGUI_CPP="$IMGUI_PREFIX/imgui.cpp"
IMGUI_IMGUI_H="$IMGUI_PREFIX/imgui.h"
IMGUI_STB_RECTPACK_H="$IMGUI_PREFIX/imstb_rectpack.h"
IMGUI_STB_TEXTEDIT_H="$IMGUI_PREFIX/imstb_textedit.h"
IMGUI_STB_TRUETYPE_H="$IMGUI_PREFIX/imstb_truetype.h"

# backends
IMGUI_BACKEND_OPENGL3_CPP="$IMGUI_PREFIX/backends/imgui_impl_opengl3.cpp"
IMGUI_BACKEND_OPENGL3_H="$IMGUI_PREFIX/backends/imgui_impl_opengl3.h"
IMGUI_BACKEND_OPENGL3_LOADER_H="$IMGUI_PREFIX/backends/imgui_impl_opengl3_loader.h"

# license
IMGUI_LICENSE="$IMGUI_PREFIX/LICENSE.txt"

# output directory
OUTPUT=./libs/imgui

# curl all files
curl $IMGUI_CONFIG_H > $OUTPUT/imconfig.h
curl $IMGUI_DRAW_CPP > $OUTPUT/imgui_draw.cpp
curl $IMGUI_INTERNAL_H > $OUTPUT/imgui_internal.h
curl $IMGUI_TABLES_CPP > $OUTPUT/imgui_tables.cpp
curl $IMGUI_WIDGETS_CPP > $OUTPUT/imgui_widgets.cpp
curl $IMGUI_IMGUI_CPP > $OUTPUT/imgui.cpp
curl $IMGUI_IMGUI_H > $OUTPUT/imgui.h
curl $IMGUI_STB_RECTPACK_H > $OUTPUT/imstb_rectpack.h
curl $IMGUI_STB_TEXTEDIT_H > $OUTPUT/imstb_textedit.h
curl $IMGUI_STB_TRUETYPE_H > $OUTPUT/imstb_truetype.h

curl $IMGUI_BACKEND_OPENGL3_CPP > $OUTPUT/imgui_impl_opengl3.cpp
curl $IMGUI_BACKEND_OPENGL3_H > $OUTPUT/imgui_impl_opengl3.h
curl $IMGUI_BACKEND_OPENGL3_LOADER_H > $OUTPUT/imgui_impl_opengl3_loader.h


# license
curl $IMGUI_LICENSE > $OUTPUT/LICENSE.txt