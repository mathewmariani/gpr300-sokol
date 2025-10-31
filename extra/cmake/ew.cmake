set(BUILD_WITH_EW_DEFAULT TRUE)

option(BUILD_WITH_EW "ew Enabled" ${BUILD_WITH_EW_DEFAULT})
message("BUILD_WITH_EW: ${BUILD_WITH_EW}")

if(BUILD_WITH_EW)
  set(EW_DIR ${THIRDPARTY_DIR}/ew)
  set(EW_FILES
    ${EW_DIR}/mesh.cpp
    ${EW_DIR}/mesh.h
    ${EW_DIR}/model.cpp
    ${EW_DIR}/model.h
    ${EW_DIR}/procGen.cpp
    ${EW_DIR}/procGen.h
    ${EW_DIR}/shader.cpp
    ${EW_DIR}/shader.h
    ${EW_DIR}/texture.cpp
    ${EW_DIR}/texture.h
    ${EW_DIR}/transform.h)

  add_library(ew STATIC ${EW_FILES})

  target_link_libraries(ew PRIVATE assimp)
  target_link_libraries(ew PRIVATE batteries)
  target_link_libraries(ew PRIVATE fast_obj)
  target_link_libraries(ew PRIVATE glm)
  target_link_libraries(ew PRIVATE stb)
endif()