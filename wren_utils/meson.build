utils = library(
    'wren_utils',
    files(
        'src/filesystem.cpp',
        'src/result.cpp',
        'src/string.cpp',
        'src/string_reader.cpp',
    ),
    include_directories: ['include', 'include/wren/utils'],
    dependencies: [boost],
)
wren_utils_dep = declare_dependency(
    include_directories: 'include',
    dependencies: [boost],
    link_with: utils,
)

subdir('tests')
