#pragma once

#include <vk_mem_alloc.h>

#include <memory>
#include <vulkan/vulkan.hpp>

namespace wren {

class GraphicsContext;

class Buffer {
 public:
  auto Create(const std::shared_ptr<GraphicsContext>& context, size_t size, vk::BufferUsageFlags usage);

 private:
  VkBuffer buffer{};
  VmaAllocation allocation{};
};

}  // namespace wren
