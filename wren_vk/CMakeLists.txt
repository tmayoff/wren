add_library(wren_vk src/errors.cpp src/buffer.cpp)

target_include_directories(wren_vk PUBLIC include)
target_include_directories(wren_vk PRIVATE include/wren_vk)
target_link_libraries(wren_vk wren_utils vulkan)
