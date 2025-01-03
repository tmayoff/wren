project('wren', 'cpp', version: '0.0.1', default_options: 'cpp_std=c++23')

cpp = meson.get_compiler('cpp')

buildtype = get_option('buildtype')

fmt = dependency('fmt')
spdlog = dependency('spdlog')
vulkan = dependency('vulkan')
# fontconfig = dependency('Fontconfig')
vma = dependency(
    'VulkanMemoryAllocator',
    modules: ['GPUOpen::VulkanMemoryAllocator'],
)
vulkan = dependency('vulkan')
shaderc = dependency('shaderc')
sdl2 = dependency('SDL2')
boost = dependency('boost')
boost_test = dependency('boost', modules: ['unit_test_framework'])
spirv = dependency('SPIRV-Headers')
tracy = dependency('tracy')
imgui = dependency('imgui_docking')
flecs = dependency('flecs')
toml = dependency('tomlplusplus')
# json = dependency('nlohmann_json')

add_project_arguments(
    '-DVULKAN_HPP_NO_EXCEPTIONS',
    '-DVUKLAN_HPP_ASSERT_ON_RESULT(x)',
    '-DVULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1',
    language: 'cpp',
)

if get_option('buildtype') == 'debug'
    add_project_arguments('-DWREN_DEBUG', language: 'cpp')
endif

if get_option('profiling')
    add_project_arguments('-DWREN_PROFILING', '-DTRACY_ENABLE', language: 'cpp')
endif

if get_option('build_profiling')
    add_project_arguments('-ftime-trace', language: 'cpp')
endif

subdir('wren_utils')
subdir('wren_math')
subdir('wren_reflect')
# subdir('wren_text')
subdir('wren_vk')
# subdir('wren_gltf')
# subdir('wren_gui')
subdir('wren')
subdir('wren_physics')
# subdir('wren_ecs')
subdir('editor')
