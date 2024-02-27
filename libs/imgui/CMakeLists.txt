set(IMGUI_FILES
    imconfig.h
    imgui_draw.cpp
    imgui_demo.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp
    imgui.h 
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h)

add_library(imgui STATIC ${IMGUI_FILES})