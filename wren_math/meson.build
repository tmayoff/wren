wrenm = library(
    'wren_math',
    'src/geometry.cpp',
    'src/utils.cpp',
    include_directories: ['include', 'include/wren/math'],
    install: true,
)
wrenm_dep = declare_dependency(include_directories: 'include', link_with: wrenm)

subdir('tests')
