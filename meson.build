project('wren', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++23'])

add_project_arguments('-U_FORTIFY_SOURCE', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_EXCEPTIONS', language: 'cpp')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DWREN_DEBUG', language: 'cpp')
endif

boost = dependency('boost')
spdlog = dependency('spdlog')
sdl = dependency('SDL2')
expected = dependency('tl-expected')
vulkan = dependency('vulkan')

subdir('wren')
subdir('editor')
