set(BUILD_WITH_TRACY_DEFAULT TRUE)

option(BUILD_WITH_TRACY "tracy Enabled" ${BUILD_WITH_TRACY_DEFAULT})
message("BUILD_WITH_TRACY: ${BUILD_WITH_TRACY}")

if(BUILD_WITH_TRACY)
  add_definitions(-DTRACY_ENABLE)
  set(TRACY_DIR ${THIRDPARTY_DIR}/tracy)
  file(GLOB TRACY_CLIENT "${THIRDPARTY_DIR}/client/*")
  file(GLOB TRACY_COMMON "${THIRDPARTY_DIR}/common/*")
  set(TRACY_FILES
    ${TRACY_DIR}/TracyClient.cpp)
  
  include_directories(${TRACY_DIR})
  include_directories(${TRACY_DIR}/client)
  include_directories(${TRACY_DIR}/common)

  add_library(tracy STATIC ${TRACY_FILES})
endif()