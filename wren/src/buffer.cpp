#include "buffer.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren {

auto Buffer::Create(VmaAllocator const& allocator, size_t size,
                    VkBufferUsageFlags usage)
    -> std::shared_ptr<Buffer> {
  auto b = std::make_shared<Buffer>();

  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = static_cast<VkBufferUsageFlags>(usage);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
  alloc_info.flags =
      VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

  VkBuffer buf{};
  vmaCreateBuffer(allocator, &create_info, &alloc_info, &buf,
                  &b->allocation, nullptr);
  b->buffer = buf;

  return b;
}

}  // namespace wren
