cmake_minimum_required(VERSION 3.5)

project(gpr300)

set(THIRDPARTY_DIR libs)
set(ASSIGNMENTS_DIR assignments)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/assignments")

# suppress warnings
add_compile_options(-Wno-c99-designator)
add_compile_options(-Wno-reorder-init-list)

# macros
include("extra/cmake/macros.cmake")
include("extra/cmake/batteries.cmake")
include("extra/cmake/dbgui.cmake")
include("extra/cmake/fast_obj.cmake")
include("extra/cmake/glm.cmake")
include("extra/cmake/imgui.cmake")
include("extra/cmake/sokol.cmake")
include("extra/cmake/stb.cmake")

# libraries
include_directories(${THIRDPARTY_DIR})

# assignments
add_subdirectory(${ASSIGNMENTS_DIR})
set_property(DIRECTORY ${ASSIGNMENTS_DIR} PROPERTY FOLDER "assignments")

# assets
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_SOURCE_DIR}/assignments/assets"
  "${CMAKE_BINARY_DIR}/assignments/${CMAKE_BUILD_TYPE}/assets")