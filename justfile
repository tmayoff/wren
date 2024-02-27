default: build

build:
    ninja -C build

run: build
    ./build/editor/wren_editor

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

clean:
    meson setup build --wipe
