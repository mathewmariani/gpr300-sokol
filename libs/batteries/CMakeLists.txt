process_shaders()

set(BATTERIES_FILES
    assets.cpp
    assets.h
    camera.cpp
    camera.h
    depthbuffer.h
    framebuffer.cpp
    framebuffer.h
    geometrybuffer.h
    gizmo.h
    lights.h
    materials.h
    math.h
    mesh.h
    model.cpp
    model.h
    pass.h
    postprocess.h
    scene.cpp
    scene.h
    shape.cpp
    shape.h
    skybox.cpp
    skybox.h
    texture.h
    transform.h
    vertex.h)

add_library(batteries STATIC ${BATTERIES_FILES})
target_link_libraries(batteries PRIVATE fast_obj)
target_link_libraries(batteries PRIVATE glm)
target_link_libraries(batteries PRIVATE sokol)