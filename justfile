default: build

configure:
    cmake -S. -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=On

build:
    cmake --build build

run: build
    ./build/editor/wren_editor

test: build
    cd build && ctest --output-on-failure

run_renderdoc:
    ENABLE_VULKAN_RENDERDOC_CAPTURE=1 ./build/editor/wren_editor

gdb: build
    gdb --tui --args ./build/editor/wren_editor

clean:
    rm -rf build

everything: deps configure build

deps:
    conan install . --build=missing -s build_type=Debug
