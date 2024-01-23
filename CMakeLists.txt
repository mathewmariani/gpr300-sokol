cmake_minimum_required(VERSION 3.5)

project(gpr300)

# platform selection
add_definitions(-DSOKOL_NO_DEPRECATED)
if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
  add_definitions(-DSOKOL_GLES3)
elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
  add_definitions(-DSOKOL_GLCORE33)
endif()

# libraries
include_directories(libs)
add_subdirectory(libs)
set_property(DIRECTORY libs PROPERTY FOLDER "libs")

# assignments
add_subdirectory(assignments)
set_property(DIRECTORY assignments PROPERTY FOLDER "assignments")

# assets
file(COPY "assignments/assets" DESTINATION ${CMAKE_BINARY_DIR}/assignments)
