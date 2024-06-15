wrenm = library(
    'wrenm',
    'src/geometry.cpp',
    'src/utils.cpp',
    include_directories: ['include', 'include/wren_math'],
)
wrenm_dep = declare_dependency(include_directories: 'include', link_with: wrenm)

subdir('tests')
