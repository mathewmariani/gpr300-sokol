set(BUILD_WITH_TRACY_DEFAULT TRUE)

option(BUILD_WITH_TRACY "tracy Enabled" ${BUILD_WITH_TRACY_DEFAULT})
message("BUILD_WITH_TRACY: ${BUILD_WITH_TRACY}")

if(BUILD_WITH_TRACY)
  add_definitions(-DTRACY_ENABLE)
  set(TRACY_DIR ${THIRDPARTY_DIR}/tracy)
  file(GLOB TRACY_CLIENT "${THIRDPARTY_DIR}/client/*")
  file(GLOB TRACY_COMMON "${THIRDPARTY_DIR}/common/*")
  file(GLOB TRACY_TRACY "${THIRDPARTY_DIR}/tracy/*")
  set(TRACY_FILES
    ${TRACY_DIR}/TracyClient.cpp)
  
  include_directories(${TRACY_DIR})
  include_directories(${TRACY_DIR}/client)
  include_directories(${TRACY_DIR}/common)
  include_directories(${TRACY_DIR}/tracy)

  add_library(tracy STATIC ${TRACY_FILES})

  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    target_link_libraries(tracy PRIVATE ws2_32 dbghelp)
  endif()

  target_include_directories(tracy PUBLIC ${TRACY_DIR}/tracy)
endif()