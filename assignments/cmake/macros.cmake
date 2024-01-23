macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${target} PUBLIC -sUSE_WEBGL2=1 --shell-file ../../extra/shell.html)
  endif()
endmacro()

macro(add_assignment target file)
  add_executable(${target} ${file})
  target_link_libraries(${target} fast_obj)
  target_link_libraries(${target} glm)
  target_link_libraries(${target} imgui)
  target_link_libraries(${target} sokol)
  target_link_libraries(${target} stb)  
  target_include_directories(${target} PUBLIC ../libs)

  target_compile_options(${target} PRIVATE
    -Waddress-of-temporary
    -Wreorder-init-list
    -Wc99-designator)

  emscripten(${target})
endmacro()
