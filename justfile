default: build run

build:
    ninja -C build

run:
    ./build/editor/wren_editor

clean:
    meson setup build --wipe
