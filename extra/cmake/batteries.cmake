set(BUILD_WITH_BATTERIES_DEFAULT TRUE)

option(BUILD_WITH_BATTERIES "batteries Enabled" ${BUILD_WITH_BATTERIES_DEFAULT})
message("BUILD_WITH_BATTERIES: ${BUILD_WITH_BATTERIES}")

if(BUILD_WITH_BATTERIES)
  set(BATTERIES_DIR ${THIRDPARTY_DIR}/batteries)
  set(BATTERIES_FILES
    ${BATTERIES_DIR}/assets.cpp
    ${BATTERIES_DIR}/assets.h
    ${BATTERIES_DIR}/camera.cpp
    ${BATTERIES_DIR}/camera.h
    ${BATTERIES_DIR}/depthbuffer.h
    ${BATTERIES_DIR}/framebuffer.cpp
    ${BATTERIES_DIR}/framebuffer.h
    ${BATTERIES_DIR}/geometrybuffer.h
    ${BATTERIES_DIR}/gizmo.h
    ${BATTERIES_DIR}/lights.h
    ${BATTERIES_DIR}/materials.h
    ${BATTERIES_DIR}/math.h
    ${BATTERIES_DIR}/mesh.h
    ${BATTERIES_DIR}/model.cpp
    ${BATTERIES_DIR}/model.h
    ${BATTERIES_DIR}/pass.h
    ${BATTERIES_DIR}/postprocess.h
    ${BATTERIES_DIR}/scene.cpp
    ${BATTERIES_DIR}/scene.h
    ${BATTERIES_DIR}/shape.cpp
    ${BATTERIES_DIR}/shape.h
    ${BATTERIES_DIR}/skybox.cpp
    ${BATTERIES_DIR}/skybox.h
    ${BATTERIES_DIR}/texture.h
    ${BATTERIES_DIR}/transform.h
    ${BATTERIES_DIR}/vertex.h)

  add_library(batteries STATIC ${BATTERIES_FILES})
  # target_include_directories(batteries INTERFACE ${BATTERIES_DIR})

  target_link_libraries(batteries PRIVATE fast_obj)
  target_link_libraries(batteries PRIVATE glm)
  target_link_libraries(batteries PRIVATE sokol)
endif()