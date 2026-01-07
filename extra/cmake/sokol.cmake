set(BUILD_WITH_SOKOL_DEFAULT TRUE)

option(BUILD_WITH_SOKOL "sokol Enabled" ${BUILD_WITH_SOKOL_DEFAULT})
message("BUILD_WITH_SOKOL: ${BUILD_WITH_SOKOL}")

if(BUILD_WITH_SOKOL)
  #defines
  if (CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    add_definitions(-DSOKOL_GLES3)
  elseif (CMAKE_SYSTEM_NAME STREQUAL Darwin)
    add_definitions(-DSOKOL_GLCORE)
  elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    add_definitions(-DSOKOL_GLCORE)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
  else()
    add_definitions(-DSOKOL_GLCORE)
  endif()

  set(SOKOL_DIR ${THIRDPARTY_DIR}/sokol)
  set(SOKOL_SRC ${SOKOL_DIR}/sokol.c)

  add_library(sokol STATIC ${SOKOL_SRC})
  target_include_directories(sokol INTERFACE ${SOKOL_DIR})

  if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # compile sokol.c as Objective-C
    target_compile_options(sokol PRIVATE -x objective-c)
    target_link_libraries(sokol
        "-framework QuartzCore"
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework AudioToolbox")
  elseif (CMAKE_SYSTEM_NAME STREQUAL Linux)
    target_link_libraries(sokol INTERFACE X11 Xi Xcursor GL asound dl m)
    target_link_libraries(sokol PUBLIC Threads::Threads)
  elseif (CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(sokol PUBLIC kernel32 user32 shell32)
  endif()
endif()