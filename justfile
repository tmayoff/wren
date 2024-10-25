default: build

# configure:
#     meson setup build

# reconfigure:
#     meson setup build --wipe

build:
    xmake

run: build
    xmake run editor ~/Documents/wren_game/

debug: build
    xmake run -d editor ~/Documents/wren_game

test: build
    
clean:
    ninja -C build clean

build_docs:
	doxygen
