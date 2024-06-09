# imgui = dependency('imgui')

wren_gui = library(
    'wren_gui',
    'src/instance.cpp',
    include_directories: ['include', 'include/wren_gui'],
    dependencies: [
        wren_vk_dep,
        wrenm_dep,
        wren_utils_dep,
        wren_text_dep,
        spdlog,
        # imgui,
    ],
)
wren_gui_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_gui,
    dependencies: [
        wren_vk_dep,
        wrenm_dep,
        wren_utils_dep,
        wren_text_dep,
        spdlog,
        # imgui,
    ],
)
