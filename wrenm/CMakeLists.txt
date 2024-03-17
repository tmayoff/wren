add_library(wrenm src/utils.cpp)

target_include_directories(wrenm PUBLIC include)
target_include_directories(wrenm PRIVATE include/wrenm)

add_subdirectory(tests)
