set(BUILD_WITH_FASTOBJ_DEFAULT TRUE)

option(BUILD_WITH_FASTOBJ "fast_obj Enabled" ${BUILD_WITH_FASTOBJ_DEFAULT})
message("BUILD_WITH_FASTOBJ: ${BUILD_WITH_FASTOBJ}")

if(BUILD_WITH_FASTOBJ)
  set(FASTOBJ_DIR ${THIRDPARTY_DIR}/fast_obj)
  set(FASTOBJ_FILES
    ${FASTOBJ_DIR}/fast_obj.c
    ${FASTOBJ_DIR}/fast_obj.h)

  add_library(fast_obj STATIC ${FASTOBJ_FILES})
  # target_include_directories(fast_obj INTERFACE ${FASTOBJ_DIR})
endif()