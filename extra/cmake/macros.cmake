macro(emscripten target)
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    target_link_options(${target} PRIVATE
      -sINITIAL_MEMORY=50MB
      -sMAXIMUM_MEMORY=200MB
      -sALLOW_MEMORY_GROWTH=1
      -sUSE_WEBGL2=1
      $<$<CONFIG:Debug>:-g>
      $<$<CONFIG:Debug>:--shell-file ../extra/shell.html>)
  endif()
endmacro()

macro(add_assignment target sources)
  add_executable(${target} ${sources} ../boilerplate.cpp)
  target_link_libraries(${target} PRIVATE batteries)
  target_link_libraries(${target} PRIVATE fast_obj)
  target_link_libraries(${target} PRIVATE glm)
  target_link_libraries(${target} PRIVATE imgui)
  target_link_libraries(${target} PRIVATE sokol)
  target_link_libraries(${target} PRIVATE stb)
  target_include_directories(${target} PUBLIC .)
  target_include_directories(${target} PUBLIC ../../libs)

  process_shaders()
  emscripten(${target})
endmacro()

macro(process_shaders)
  # glob all *.{vs, fs} files
  file(GLOB_RECURSE SHADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/*.vs
    ${CMAKE_CURRENT_LIST_DIR}/*.fs)

  # get a list of shaders, without duplicates
  set(SHADER_NAMES )
  foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    list(APPEND SHADER_NAMES ${SHADER_NAME})
  endforeach()
  list(REMOVE_DUPLICATES SHADER_NAMES)

  # create header file containing contents of shader stages
  foreach(SHADER_NAME ${SHADER_NAMES})
    set(VS_INPUT_FILE ${CMAKE_CURRENT_LIST_DIR}/${SHADER_NAME}.vs)
    set(FS_INPUT_FILE ${CMAKE_CURRENT_LIST_DIR}/${SHADER_NAME}.fs)

    file(READ ${VS_INPUT_FILE} VS_GLSL)
    file(READ ${FS_INPUT_FILE} FS_GLSL)

    string(REPLACE "." "_" SHADER_NAME ${SHADER_NAME})
    string(REPLACE "-" "_" SHADER_NAME ${SHADER_NAME})
    configure_file(../../extra/cmake/shader_template.h.in ${CMAKE_CURRENT_LIST_DIR}/${SHADER_NAME}.glsl.h)
  endforeach()
endmacro()