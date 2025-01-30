cmake_minimum_required(VERSION 3.5)

project(gpr300)

set(THIRDPARTY_DIR libs)
set(ASSIGNMENTS_DIR assignments)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/assignments")

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

# assets
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}"
  "${CMAKE_CURRENT_SOURCE_DIR}/website/public/demos")

execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_CURRENT_SOURCE_DIR}/assets"
  "${CMAKE_CURRENT_SOURCE_DIR}/website/public/assets")