wren_gltf = static_library(
    'gltf',
    files('src/gltf.cpp'),
    dependencies: [spdlog, json],
    include_directories: ['include', 'include/wren/gltf'],
)


wren_gltf_dep = declare_dependency(
    include_directories: ['include', 'include/wren/gltf'],
    link_with: wren_gltf,
)
