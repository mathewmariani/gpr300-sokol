set(BUILD_WITH_GLM_DEFAULT TRUE)

option(BUILD_WITH_GLM "batteries Enabled" ${BUILD_WITH_GLM_DEFAULT})
message("BUILD_WITH_GLM: ${BUILD_WITH_GLM}")

if(BUILD_WITH_GLM)
  set(GLM_DIR ${THIRDPARTY_DIR}/glm)
  add_subdirectory(${GLM_DIR})
endif()