set(BUILD_WITH_IMGUIZMO_DEFAULT TRUE)

option(BUILD_WITH_IMGUIZMO "imguizmo Enabled" ${BUILD_WITH_IMGUIZMO_DEFAULT})
message("BUILD_WITH_IMGUIZMO: ${BUILD_WITH_IMGUIZMO}")

if(BUILD_WITH_IMGUIZMO)
  set(IMGUIZMO_DIR ${THIRDPARTY_DIR}/imguizmo)
  file(GLOB IMGUIZMO_SRC
    ${IMGUIZMO_DIR}/*.cpp
    ${IMGUIZMO_DIR}/*.h)

  add_library(imguizmo STATIC ${IMGUIZMO_SRC})

  target_link_libraries(imguizmo PRIVATE imgui)
  target_include_directories(imguizmo PRIVATE ${IMGUI_DIR})
endif()