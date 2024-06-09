default: build

configure:
    meson setup build

reconfigure:
    meson setup build --wipe

build:
    meson compile -C build

nix_run: build
    nixVulkanIntel ./build/editor/editor

run: build
    ./build/editor/editor

test: build
    cd build && meson test

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

gdb: build
    gdb --tui --args ./build/editor/wren_editor

clean:
    ninja -C build clean

everything: configure build

