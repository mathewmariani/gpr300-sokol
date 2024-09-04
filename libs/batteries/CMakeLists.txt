process_shaders()

set(BATTERIES_FILES
    assets.cpp
    assets.h
    blinnphong.cpp
    blinnphong.h
    camera.cpp
    camera.h
    framebuffer.cpp
    framebuffer.h
    geometry.cpp
    geometry.h
    gizmo.cpp
    gizmo.h
    lighting.cpp
    lighting.h
    lights.h
    materials.h
    mesh.h
    model.h
    pass.h
    scene.cpp
    scene.h
    shape.h
    skybox.cpp
    skybox.h
    transform.h
    vertex.h)

add_library(batteries STATIC ${BATTERIES_FILES})
target_link_libraries(batteries PRIVATE fast_obj)
target_link_libraries(batteries PRIVATE glm)
target_link_libraries(batteries PRIVATE sokol)