project('wren', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++23'])

add_project_arguments('-U_FORTIFY_SOURCE', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_EXCEPTIONS', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_ASSERT_ON_RESULT(x)', language: 'cpp')

if get_option('buildtype') == 'debug'
  add_project_arguments('-DWREN_DEBUG', language: 'cpp')
endif

cpp = meson.get_compiler('cpp')

boost = dependency('boost')
spdlog = dependency('spdlog')
sdl = dependency('SDL2')
expected = dependency('tl-expected', modules: ['tl::expected'])
vulkan = dependency('vulkan')
gsl = dependency('gsl-lite', modules: ['gsl::gsl-lite'])
shaderc = dependency('shaderc')
spirv_cross_c = dependency('spirv-cross-c', static: true)

#subdir('external')

subdir('wren')
subdir('editor')
