project('wren', 'cpp', version: '0.0.1', default_options: 'cpp_std=c++20')

spdlog = dependency('spdlog')
vulkan = dependency('vulkan')
fontconfig = dependency('Fontconfig')
vma = dependency('vulkan-memory-allocator')
vulkan = dependency('vulkan')
shaderc = dependency('shaderc')
sdl2 = dependency('SDL2')

add_project_arguments(
    '-DVULKAN_HPP_NAMESPACE=_VK_',
    '-DVULKAN_HPP_NO_EXCEPTIONS',
    '-DVK_NS=_VK_',
    '-ftime-trace',
    language: 'cpp',
)

subdir('wrenm')
subdir('wren_utils')
subdir('wren_reflect')
subdir('wren_text')
subdir('wren_vk')
subdir('wren_gui')
subdir('wren')
subdir('editor')
