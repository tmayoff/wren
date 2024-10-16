utils = library(
    'wren_utils',
    files(
        'src/enums.cpp',
        'src/errors.cpp',
        'src/string.cpp',
    ),
    include_directories: ['include', 'include/wren/utils'],
    dependencies: [fmt, boost, expected],
)
wren_utils_dep = declare_dependency(
    include_directories: 'include',
    dependencies: [fmt, boost, expected],
    link_with: utils,
)
