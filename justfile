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

coverage: test
    lcov --directory . --capture --initial --output-file build/initial.info --gcov-tool ./tools/gcov.sh --ignore-errors source --ignore-errors inconsistent
    lcov --directory . --capture --output-file build/coverage.info --gcov-tool ./tools/gcov.sh --ignore-errors source --ignore-errors inconsistent
    lcov --directory . -a build/coverage.info -a build/initial.info --gcov-tool ./tools/gcov.sh --ignore-errors source --ignore-errors inconsistent
    # lcov --remove build/coverage.info '*/tests/*' '/nix/*' --output-file build/coverage.info --ignore-errors source --ignore-errors inconsistent
    
clean:
    ninja -C build clean

build_docs:
	doxygen
