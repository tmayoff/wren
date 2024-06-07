wren_text = library(
    'wren_text',
    'src/font.cpp',
    include_directories: ['include', 'include/wren_text'],
    dependencies: [fontconfig, wren_utils_dep, spdlog],
)
wren_text_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_text,
)
