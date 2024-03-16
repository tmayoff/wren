add_library(
  wren
  src/application.cpp
  src/buffer.cpp
  src/event.cpp
  src/graph.cpp
  src/graphics_context.cpp
  src/memory.cpp
  src/memory.cpp
  src/render_pass.cpp
  src/renderer.cpp
  src/shader.cpp
  src/utils/device.cpp
  src/utils/queue.cpp
  src/utils/vulkan.cpp
  src/utils/vulkan_errors.cpp
  src/window.cpp)

# target_sources(wren PUBLIC FILE_SET CXX_MODULES FILES)

target_compile_definitions(wren PUBLIC VULKAN_HPP_NO_EXCEPTIONS)
target_include_directories(wren PUBLIC include)
target_include_directories(wren PRIVATE include/wren)

target_link_libraries(
  wren
  PUBLIC wren_reflect
         wrenm
         SDL2
         vulkan
         ${shaderc_LIBRARIES}
         spdlog::spdlog
         tl::expected)
