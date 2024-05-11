wren = library(
    'wren',
    'src/application.cpp',
    include_directories: 'include',
    dependencies: [
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
    ],
)
wren_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren,
    dependencies: [
        vulkan,
        wren_vk_dep,
        wren_reflect_dep,
        wrenm_dep,
    ],
)
