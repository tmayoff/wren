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
    'src/window.cpp',
    'src/utils/vulkan.cpp',
    'src/utils/queue.cpp',
    'src/utils/device.cpp',

    include_directories: ['include', 'include/wren'],
    dependencies: [
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        sdl2,
        tracy,
    ],
    install: true
)
wren_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren,
    dependencies: [
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        sdl2,
    ],
)
