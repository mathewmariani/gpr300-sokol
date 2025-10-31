set(BUILD_WITH_BATTERIES_DEFAULT TRUE)

option(BUILD_WITH_BATTERIES "batteries Enabled" ${BUILD_WITH_BATTERIES_DEFAULT})
message("BUILD_WITH_BATTERIES: ${BUILD_WITH_BATTERIES}")

if(BUILD_WITH_BATTERIES)
  set(BATTERIES_DIR ${THIRDPARTY_DIR}/batteries)
  set(BATTERIES_FILES
    ${BATTERIES_DIR}/camera.cpp
    ${BATTERIES_DIR}/camera.h
    ${BATTERIES_DIR}/lights.h
    ${BATTERIES_DIR}/materials.h
    ${BATTERIES_DIR}/math.cpp
    ${BATTERIES_DIR}/math.h
    ${BATTERIES_DIR}/opengl.h
    ${BATTERIES_DIR}/scene.cpp
    ${BATTERIES_DIR}/scene.h)

  add_library(batteries STATIC ${BATTERIES_FILES})

  target_link_libraries(batteries PRIVATE glm)
  target_link_libraries(batteries PRIVATE sokol)
endif()