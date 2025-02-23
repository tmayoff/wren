wren_reflect = library(
    'wren_reflect',
    'src/spirv_reflect.cpp',
    'src/reflect.cpp',
    include_directories: 'include',
    install: true,
    dependencies: [wren_utils_dep, spirv],
    cpp_args: ['-DSPV_ENABLE_UTILITY_CODE'],
)
wren_reflect_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_reflect,
    dependencies: [wren_utils_dep, spirv],
)

subdir('tests')
