add_library(wren_utils src/enums.cpp)
target_include_directories(wren_utils PUBLIC include)
target_include_directories(wren_utils PRIVATE include/wren_utils)
