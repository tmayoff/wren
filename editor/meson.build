deps = [wren_dep, imgui, tracy]

shaders = custom_target(
    command: [
        slangc,
        '-o', '@OUTPUT@',
        '-target', 'spirv',
        '-source-embed-language', 'cpp',
        '-source-embed-name', 'editor_mesh',
        '-source-embed-style', 'default',
        '-matrix-layout-row-major',
        '-preserve-params', '@INPUT@',
    ],
    input: ['assets/shaders/editor_mesh.slang'],
    output: ['editor_mesh.hpp'],
)

executable(
    'wren_editor',
    'src/main.cpp',
    'src/editor.cpp',
    'src/ui.cpp',
    'src/scene_panel.cpp',
    'src/inspector_panel.cpp',
    'src/filesystem_panel.cpp',
    shaders,
    dependencies: deps,
    install: true,

    cpp_args: [
        '-DWREN_BUILD_ASSETS_DIR="@0@/assets"'.format(
            meson.current_source_dir(),
        ),
    ],
)
