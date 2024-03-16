#include "buffer.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren {

auto Buffer::Create(const VmaAllocator& allocator, size_t size,
                    vk::BufferUsageFlags usage)
    -> std::shared_ptr<Buffer> {
  auto b = std::make_shared<Buffer>();

  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = static_cast<VkBufferUsageFlags>(usage);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

  vmaCreateBuffer(allocator, &create_info, &alloc_info, &b->buffer,
                  &b->allocation, nullptr);
  return b;
}

void Buffer::set_data_raw(VmaAllocator allocator, size_t size,
                          void* data) {
  vmaCopyAllocationToMemory(allocator, allocation, 0, data, size);
}

}  // namespace wren
