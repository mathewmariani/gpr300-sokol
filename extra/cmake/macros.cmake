if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
  set(IS_EMSCRIPTEN 1)
endif()

macro(emscripten target)
  if (IS_EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    target_link_options(${target} PRIVATE
      -sINITIAL_MEMORY=50MB
      -sMAXIMUM_MEMORY=200MB
      -sALLOW_MEMORY_GROWTH=1
      -sUSE_WEBGL2=1
      -sSINGLE_FILE=1
      -sFORCE_FILESYSTEM=1
      $<$<CONFIG:Debug>:-g>)
  endif()
endmacro()

macro(copy_assets)
  if (IS_EMSCRIPTEN)
    # Paths
    set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
    set(OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/")
    set(ASSET_PACKAGE_JS "${OUTPUT_DIR}/assets.js")
    set(ASSET_PACKAGE_DATA "${OUTPUT_DIR}/assets.data")

    # Emscripten file_packager.py
    set(FILE_PACKAGER "${EMSCRIPTEN_ROOT_PATH}/tools/file_packager.py")

    if(NOT EXISTS "${FILE_PACKAGER}")
      message(FATAL_ERROR "file_packager.py not found at ${FILE_PACKAGER}")
    endif()

    # Python interpreter (CMake 3.12+)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_custom_command(
      OUTPUT "${ASSET_PACKAGE_JS}" "${ASSET_PACKAGE_DATA}"
      COMMAND ${CMAKE_COMMAND} -E echo "Packing shared assets for Emscripten..."
      COMMAND "${Python3_EXECUTABLE}" "${FILE_PACKAGER}"
              "${ASSET_PACKAGE_DATA}"
              --preload "${ASSETS_DIR}@assets"
              --js-output="${ASSET_PACKAGE_JS}"
      WORKING_DIRECTORY "${OUTPUT_DIR}"
      DEPENDS ${ASSETS_DIR}
      COMMENT "Creating Emscripten asset package: assets.data + assets.js")

    add_custom_target(copy_assets ALL
      DEPENDS "${ASSET_PACKAGE_JS}" "${ASSET_PACKAGE_DATA}")

  else()
    add_custom_target(copy_assets ALL
      COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets"
      COMMAND ${CMAKE_COMMAND} -E create_symlink
              "${CMAKE_SOURCE_DIR}/assets"
              "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets"
      COMMENT "Linking shared assets directory → ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets")
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
  target_link_libraries(${target} PRIVATE imguizmo)

  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(${target} PRIVATE gl3w)
  endif()

  # include directories
  target_include_directories(${target} PUBLIC .)
  target_include_directories(${target} PUBLIC ../../libs)

  emscripten(${target})
endmacro()