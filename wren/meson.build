sources = []

deps = [
    boost,
    expected,
    spdlog,
    sdl,
    wrenm_dep,
    wren_reflect_dep,
    vulkan,
    shaderc,
    tracy,
]

subdir('src')

wren_lib = library(
    'wren',
    sources,
    dependencies: deps,
    include_directories: 'include',
)
wren = declare_dependency(
    dependencies: deps,
    include_directories: 'include',
    link_with: wren_lib,
)
