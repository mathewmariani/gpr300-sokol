set(BATTERIES_FILES
    assets.cpp
    assets.h
    blinnphong.h
    camera.cpp
    camera.h
    framebuffer.cpp
    framebuffer.h
    lights.h
    materials.h
    mesh.h
    model.h
    shape.h
    transform.h
    vertex.h)

add_library(batteries STATIC ${BATTERIES_FILES})
target_link_libraries(batteries PRIVATE fast_obj)
target_link_libraries(batteries PRIVATE glm)
target_link_libraries(batteries PRIVATE sokol)