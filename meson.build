project('wren', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++23'])

add_project_arguments('-U_FORTIFY_SOURCE', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_NO_EXCEPTIONS', language: 'cpp')
add_project_arguments('-DVULKAN_HPP_ASSERT_ON_RESULT(x)', language: 'cpp')
add_project_arguments('-UFMT_EXCEPTIONS', language: 'cpp')

if get_option('buildtype') == 'debug'
    add_project_arguments('-DWREN_DEBUG', language: 'cpp')
endif

if get_option('profiling')
    add_project_arguments('-DTRACY_ENABLE', language: 'cpp')
endif

cpp = meson.get_compiler('cpp')

boost = dependency('boost')
spdlog = dependency('spdlog')
sdl = dependency('SDL2')
expected = dependency('tl-expected', modules: ['tl::expected'])
vulkan = dependency('vulkan')
shaderc = dependency('shaderc')
tracy = dependency('tracy', required: false)
if not tracy.found()
    tracy = cpp.find_library(
        'tracy',
    )
endif

subdir('wren')
subdir('editor')
