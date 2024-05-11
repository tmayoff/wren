project('wren', 'cpp', version: '0.0.1', default_options: 'cpp_std=c++20')

spdlog = dependency('spdlog')
vulkan = dependency('vulkan')

add_project_arguments(
    '-DVULKAN_HPP_NAMESPACE=_VK_',
    '-DVK_NS=_VK_',
    language: 'cpp',
)

subdir('wrenm')
subdir('wren_utils')
subdir('wren_reflect')
subdir('wren_vk')
subdir('wren_gui')
subdir('wren')
subdir('editor')
