deps = [wren_dep, imgui, tracy]

executable(
    'wren_editor',
    'src/main.cpp',
    'src/editor.cpp',
    'src/ui.cpp',
    'src/scene_panel.cpp',
    'src/inspector_panel.cpp',
    'src/filesystem_panel.cpp',
    dependencies: deps,
    install: true,

    cpp_args: [
        '-DWREN_BUILD_ASSETS_DIR="@0@/assets"'.format(
            meson.current_source_dir(),
        ),
    ],
)
