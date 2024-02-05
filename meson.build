project('wren', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++23'])

add_project_arguments('-U_FORTIFY_SOURCE', language: 'cpp')

boost = dependency('boost')
spdlog = dependency('spdlog')
sdl = dependency('SDL2')
expected = dependency('tl-expected')

subdir('wren')
subdir('editor')
