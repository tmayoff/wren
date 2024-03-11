#include "wren/buffer.hpp"

#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

#include "wren/graphics_context.hpp"

namespace wren {

auto Buffer::Create(const std::shared_ptr<GraphicsContext>& context,
                    size_t size, vk::BufferUsageFlags usage) {
  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = static_cast<VkBufferUsageFlags>(usage);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

  vmaCreateBuffer(context->allocator(), &create_info, &alloc_info,
                  &buffer, &allocation, nullptr);
}

}  // namespace wren
