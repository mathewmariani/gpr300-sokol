set(BUILD_WITH_GL3W_DEFAULT TRUE)

option(BUILD_WITH_GL3W "gl3w Enabled" ${BUILD_WITH_GL3W_DEFAULT})
message("BUILD_WITH_GL3W: ${BUILD_WITH_GL3W}")

if(BUILD_WITH_GL3W)
  set(GL3W_DIR ${THIRDPARTY_DIR}/gl3w)
  add_library(gl3w STATIC ${GL3W_DIR}/src/gl3w.c)

  target_include_directories(gl3w PUBLIC ${GL3W_DIR}/include)
endif()