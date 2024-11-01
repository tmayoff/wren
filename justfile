default: build

configure:
    meson setup build

reconfigure:
    meson setup build --wipe

build:
    meson compile -C build

run: build
    ./build/editor/wren_editor ~/Documents/wren_game

debug: build
    lldb ./build/editor/wren_editor ~/Documents/wren_game

test: build
    meson test -C build --print-errorlogs

coverage: 
    cd build && llvm-cov gcov $(find ./ "*.gcno")
    
clean:
    ninja -C build clean

build_docs:
	doxygen
