macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${target} PUBLIC -sUSE_WEBGL2=1 --shell-file ../extra/shell.html)
  endif()
endmacro()

macro(add_assignment target file)
  add_executable(${target} ${file} batteries.h)

  target_compile_options(${target} PRIVATE
    -Waddress-of-temporary
    -Wreorder-init-list
    -Wc99-designator)

  target_link_libraries(${target} PRIVATE fast_obj)
  target_link_libraries(${target} PRIVATE glm)
  target_link_libraries(${target} PRIVATE imgui)
  target_link_libraries(${target} PRIVATE sokol)
  target_link_libraries(${target} PRIVATE stb)  
  target_include_directories(${target} PUBLIC ../libs)

  emscripten(${target})
endmacro()
