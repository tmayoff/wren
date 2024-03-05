default: build

build:
    ninja -C build

run: build
    ./build/editor/wren_editor

test:
    meson test -C build --print-errorlogs

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

clean:
    meson setup build --wipe
