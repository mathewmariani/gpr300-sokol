set(BUILD_WITH_IMGUI_DEFAULT TRUE)

option(BUILD_WITH_IMGUI "imgui Enabled" ${BUILD_WITH_IMGUI_DEFAULT})
message("BUILD_WITH_IMGUI: ${BUILD_WITH_IMGUI}")

if(BUILD_WITH_IMGUI)
  set(IMGUI_DIR ${THIRDPARTY_DIR}/imgui)
  set(IMGUI_FILES
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui.h 
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h)

  add_library(imgui STATIC ${IMGUI_FILES})
  # target_include_directories(imgui INTERFACE ${IMGUI_DIR})
endif()