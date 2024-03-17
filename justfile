default: build

configure:
    cmake -S. -B build -G Ninja -DCMAKE_BUILD_TYPE=DEBUG --preset conan-release

build:
    cmake --build build

run: build
    ./build/editor/wren_editor

test: build
    cd build && ctest --output-on-failure

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

gdb:
    gdb --tui --args ./build/editor/wren_editor

clean:
    rm -rf build

deps:
    conan install . --build=missing
