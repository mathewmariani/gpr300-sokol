set(BUILD_WITH_DBGUI_DEFAULT TRUE)

option(BUILD_WITH_DBGUI "dbgui Enabled" ${BUILD_WITH_DBGUI_DEFAULT})
message("BUILD_WITH_DBGUI: ${BUILD_WITH_DBGUI}")

if(BUILD_WITH_DBGUI)
  set(DBGUI_DIR ${THIRDPARTY_DIR}/dbgui)
  set(DBGUI_FILES
    ${DBGUI_DIR}/dbgui.cpp
    ${DBGUI_DIR}/dbgui.h)

  add_library(dbgui STATIC ${DBGUI_FILES})
  target_include_directories(dbgui PUBLIC ..)

  target_link_libraries(dbgui PRIVATE imgui)
  target_link_libraries(dbgui PRIVATE sokol)
endif()