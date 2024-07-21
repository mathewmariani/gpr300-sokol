set(BATTERIES_FILES
    camera.cpp
    camera.h
    lights.h
    materials.h
    vertex.h)

add_library(batteries STATIC ${BATTERIES_FILES})
target_link_libraries(batteries PRIVATE glm)
target_link_libraries(batteries PRIVATE sokol)