add_library(wren_vk src/buffer.cpp src/errors.cpp src/shader.cpp)

target_include_directories(wren_vk PUBLIC include)
target_include_directories(wren_vk PRIVATE include/wren_vk)
target_link_libraries(wren_vk wren_utils vulkan wren_reflect spdlog::spdlog)
