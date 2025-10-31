cmake_minimum_required(VERSION 3.5)

project(gpr300)

set(THIRDPARTY_DIR libs)
set(ASSIGNMENTS_DIR assignments)

# output directoris
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/demos")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs")

# macros
include("extra/cmake/macros.cmake")
include("extra/cmake/assimp.cmake")
include("extra/cmake/batteries.cmake")
include("extra/cmake/dbgui.cmake")
include("extra/cmake/ew.cmake")
include("extra/cmake/fast_obj.cmake")
include("extra/cmake/gl3w.cmake")
include("extra/cmake/glm.cmake")
include("extra/cmake/imgui.cmake")
include("extra/cmake/sokol.cmake")
include("extra/cmake/stb.cmake")
include("extra/cmake/ufbx.cmake")

# libraries
include_directories(${THIRDPARTY_DIR})

# assignments
add_subdirectory(${ASSIGNMENTS_DIR})

# assets
copy_assets()