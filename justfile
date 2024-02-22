default: build

build:
    ninja -C build

run: build
    ./build/editor/wren_editor

clean:
    meson setup build --wipe
