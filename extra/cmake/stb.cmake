set(BUILD_WITH_STB_DEFAULT TRUE)

option(BUILD_WITH_STB "stb Enabled" ${BUILD_WITH_STB_DEFAULT})
message("BUILD_WITH_STB: ${BUILD_WITH_STB}")

if(BUILD_WITH_STB)
  set(STB_DIR ${THIRDPARTY_DIR}/stb)
  set(STB_FILES
    ${STB_DIR}/stb.c
    ${STB_DIR}/stb_image.h)

  add_library(stb STATIC ${STB_FILES})
  # target_include_directories(stb INTERFACE ${STB_DIR})
endif()