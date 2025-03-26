set(BUILD_WITH_UFBX_DEFAULT TRUE)

option(BUILD_WITH_UFBX "ufbx Enabled" ${BUILD_WITH_UFBX_DEFAULT})
message("BUILD_WITH_UFBX: ${BUILD_WITH_UFBX}")

if(BUILD_WITH_UFBX)
  set(UFBX_DIR ${THIRDPARTY_DIR}/ufbx)
  set(UFBX_FILES
    ${UFBX_DIR}/ufbx.c
    ${UFBX_DIR}/ufbx.h)

  add_library(ufbx STATIC ${UFBX_FILES})
  # target_include_directories(ufbx INTERFACE ${UFBX_DIR})
endif()