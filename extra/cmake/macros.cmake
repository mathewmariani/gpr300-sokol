macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    target_link_options(${target} PRIVATE
      -sINITIAL_MEMORY=50MB
      -sMAXIMUM_MEMORY=200MB
      -sALLOW_MEMORY_GROWTH=1
      -sUSE_WEBGL2=1
      -sSINGLE_FILE=1
      $<$<CONFIG:Debug>:-g>)
  endif()
endmacro()

macro(add_assignment target sources)
  add_executable(${target} ${sources} ../boilerplate.cpp)

  # link all libraries
  target_link_libraries(${target} PRIVATE assimp)
  target_link_libraries(${target} PRIVATE batteries)
  target_link_libraries(${target} PRIVATE dbgui)
  target_link_libraries(${target} PRIVATE ew)
  target_link_libraries(${target} PRIVATE fast_obj)
  target_link_libraries(${target} PRIVATE glm)
  target_link_libraries(${target} PRIVATE imgui)
  target_link_libraries(${target} PRIVATE sokol)
  target_link_libraries(${target} PRIVATE stb)

  # include directories
  target_include_directories(${target} PUBLIC .)
  target_include_directories(${target} PUBLIC ../../libs)

  emscripten(${target})
endmacro()