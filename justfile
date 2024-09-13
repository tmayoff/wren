default: build

configure:
    meson setup build

reconfigure:
    meson setup build --wipe

build:
    meson compile -C build

nix_run: build
    nixVulkanIntel ./build/editor/wren_editor

run: build
    ./build/editor/wren_editor

test: build
    meson test -C build --print-errorlogs

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

gdb: build
    gdb --tui --args ./build/editor/wren_editor

clean:
    ninja -C build clean

everything: configure build

