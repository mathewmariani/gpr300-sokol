cmake_minimum_required(VERSION 3.5)

project(gpr300)

# macros
include("extra/cmake/macros.cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/assignments")

#defines
add_definitions(-DSOKOL_NO_DEPRECATED)

# suppress warnings
add_compile_options(-Wno-c99-designator)
add_compile_options(-Wno-reorder-init-list)

# platform selection
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
  add_definitions(-DSOKOL_GLES3)
elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  add_definitions(-DSOKOL_GLCORE)
endif()

# libraries
include_directories(libs)
add_subdirectory(libs)
set_property(DIRECTORY libs PROPERTY FOLDER "libs")

# assignments
add_subdirectory(assignments)
set_property(DIRECTORY assignments PROPERTY FOLDER "assignments")

# assets
execute_process(
  COMMAND ${CMAKE_COMMAND} -E create_symlink
  "${CMAKE_SOURCE_DIR}/assignments/assets"
  "${CMAKE_BINARY_DIR}/assignments/${CMAKE_BUILD_TYPE}/assets"
)