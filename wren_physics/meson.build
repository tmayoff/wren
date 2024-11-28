wren_physics = static_library(
    'wren_physics',
    'src/ray.cpp',
    dependencies: [wren_dep, flecs],
    include_directories: ['include/wren', 'include/wren/physics'],
)
wren_physics_dep = declare_dependency(
    include_directories: 'include',
    link_with: wren_physics,
)
