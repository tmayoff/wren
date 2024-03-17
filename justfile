default: build

configure:
    cmake -S. -B build -G Ninja

build:
    cmake --build build

run: build
    ./build/editor/wren_editor

test: build
    cd build && ctest --output-on-failure

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

clean:
    rm -rf build
