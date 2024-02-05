project('wren', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++23'])

add_project_arguments('-U_FORTIFY_SOURCE', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_EXCEPTIONS', language: 'cpp')

boost = dependency('boost')
spdlog = dependency('spdlog')
sdl = dependency('SDL2')
expected = dependency('tl-expected')
vulkan = dependency('vulkan')

subdir('wren')
subdir('editor')
