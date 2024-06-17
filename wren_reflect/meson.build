wren_reflect = library(
    'wren_reflect',
    'src/spirv_reflect.cpp',
    include_directories: 'include',
    install: true
)
wren_reflect_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_reflect,
)
