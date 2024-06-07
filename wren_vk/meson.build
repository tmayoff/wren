wren_vk = library(
    'wren_vk',
    'src/errors.cpp',
    'src/buffer.cpp',
    'src/shader.cpp',
    'src/memory.cpp',
    include_directories: ['include', 'include/wren_vk'],
    dependencies: [
        wren_utils_dep,
        wren_reflect_dep,
        spdlog,
        vma,
        vulkan,
        shaderc,
    ],
)
wren_vk_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_vk,
    dependencies: [
        wren_utils_dep,
        wren_reflect_dep,
        spdlog,
        vma,
        vulkan,
        shaderc,
    ],
)
