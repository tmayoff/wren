utils = library(
    'wren_utils',
    'src/enums.cpp',
    include_directories: ['include', 'include/wren_utils'],
    dependencies: [fmt, boost],
)
wren_utils_dep = declare_dependency(
    include_directories: 'include',
    dependencies: [fmt, boost],
    link_with: utils,
)
