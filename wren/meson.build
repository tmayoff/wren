sources = []

deps = [boost, spirv_cross_c, expected, spdlog, sdl, vulkan, gsl, shaderc]

subdir('src')

wren_lib = library('wren', sources, dependencies: deps, include_directories: 'include')
wren = declare_dependency(dependencies: deps, include_directories: 'include', link_with: wren_lib)
