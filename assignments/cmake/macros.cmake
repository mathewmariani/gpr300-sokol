macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${target} PUBLIC -g -sUSE_WEBGL2=1 --shell-file ../extra/shell.html)
  endif()
endmacro()

macro(add_assignment target file)
  add_executable(${target} ${file} batteries.h)

  target_link_libraries(${target} PRIVATE fast_obj)
  target_link_libraries(${target} PRIVATE glm)
  target_link_libraries(${target} PRIVATE imgui)
  target_link_libraries(${target} PRIVATE sokol)
  target_link_libraries(${target} PRIVATE stb)  
  target_include_directories(${target} PUBLIC ../libs)

  target_compile_options(${target} PRIVATE
    -Waddress-of-temporary
    -Wreorder-init-list
    -Wc99-designator)

  emscripten(${target})
endmacro()

macro(process_shaders)
  file(GLOB_RECURSE SHADER_FILES ${CMAKE_CURRENT_LIST_DIR}/shaders/*.vs ${CMAKE_CURRENT_LIST_DIR}/shaders/*.fs)

  set(SHADER_NAMES )
  foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    list(APPEND SHADER_NAMES ${SHADER_NAME})
  endforeach()
  list(REMOVE_DUPLICATES SHADER_NAMES)

  set(SHADER_DIR ${CMAKE_CURRENT_LIST_DIR}/shaders)
  foreach(SHADER_NAME ${SHADER_NAMES})
    set(VS_INPUT_FILE ${SHADER_DIR}/${SHADER_NAME}.vs)
    set(FS_INPUT_FILE ${SHADER_DIR}/${SHADER_NAME}.fs)

    file(READ ${VS_INPUT_FILE} VS_GLSL)
    file(READ ${FS_INPUT_FILE} FS_GLSL)

    string(REPLACE "." "_" SHADER_NAME ${SHADER_NAME})
    string(REPLACE "-" "_" SHADER_NAME ${SHADER_NAME})
    configure_file(cmake/shader_template.h.in ${SHADER_DIR}/${SHADER_NAME}.h)
  endforeach()
endmacro()