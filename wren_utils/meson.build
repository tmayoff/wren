utils = library(
    'wren_utils',
    'src/enums.cpp',
    'src/errors.cpp',
    include_directories: ['include', 'include/wren_utils'],
    dependencies: [fmt, boost, expected],
)
wren_utils_dep = declare_dependency(
    include_directories: 'include',
    dependencies: [fmt, boost, expected],
    link_with: utils,
)
