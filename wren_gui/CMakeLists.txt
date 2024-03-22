add_library(wren_gui src/instance.cpp)

target_include_directories(wren_gui PUBLIC include)
target_include_directories(wren_gui PRIVATE include/wren_gui)
