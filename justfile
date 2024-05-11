default: build

configure:
    meson setup build

build:
    meson compile -C build

run: build
    ./build/editor/wren_editor

test: build
    cd build && meson test

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

gdb: build
    gdb --tui --args ./build/editor/wren_editor

clean:
    rm -rf build

everything: configure build

