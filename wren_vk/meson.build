wren_vk = library(
    'wren_vk',
    'src/buffer.cpp',
    'src/image.cpp',
    'src/shader.cpp',
    'src/memory.cpp',
    'src/vulkan.cpp',

    include_directories: ['include', 'include/wren/vk'],
    dependencies: [
        wren_utils_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        vma,
        vulkan,
        shaderc,
    ],
    install: true,
)
wren_vk_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_vk,
    dependencies: [
        wren_utils_dep,
        wren_reflect_dep,
        wrenm_dep,
        spdlog,
        vma,
        vulkan,
        shaderc,
    ],
)
