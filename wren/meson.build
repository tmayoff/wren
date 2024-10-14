wren = library(
    'wren',
    'src/application.cpp',
    'src/event.cpp',
    'src/graph.cpp',
    'src/graphics_context.cpp',
    'src/mesh.cpp',
    'src/pipeline.cpp',
    'src/render_pass.cpp',
    'src/renderer.cpp',
    'src/scene/scene.cpp',
    'src/scene/serialization.cpp',
    'src/window.cpp',
    'src/utils/vulkan.cpp',
    'src/utils/queue.cpp',
    'src/utils/device.cpp',

    include_directories: ['include', 'include/wren'],
    dependencies: [
        flecs,
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        sdl2,
        tracy,
        toml
    ],
    install: true,
)
wren_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren,
    dependencies: [
        flecs,
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        sdl2,
    ],
)
