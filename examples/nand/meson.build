executable(
    'nand',
    'main.cpp',
    dependencies: wren_dep,
    cpp_args: [
        '-DWREN_BUILD_ASSETS_DIR="@0@/assets"'.format(
            meson.current_source_dir(),
        ),
    ],
)
