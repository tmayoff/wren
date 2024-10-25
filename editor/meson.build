deps = [wren_dep, imgui, tracy]

if buildtype == 'debug'
    deps += backward_dep
endif

executable(
    'wren_editor',
    'src/main.cpp',
    # 'src/editor.cxx',
    'src/ui.cpp',
    'src/scene_panel.cpp',
    # 'src/inspector_panel.cxx',
    # 'src/filesystem_panel.cxx',
    dependencies: deps,
    install: true,

    cpp_args: [
        '-DWREN_BUILD_ASSETS_DIR="@0@/assets"'.format(
            meson.current_source_dir(),
        ),
    ],
)
