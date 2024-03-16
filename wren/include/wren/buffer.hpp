#pragma once

#include <vk_mem_alloc.h>

#include <memory>
#include <vulkan/vulkan.hpp>

namespace wren {

class GraphicsContext;

class Buffer {
 public:
  static auto Create(const VmaAllocator& allocator, size_t size,
                     vk::BufferUsageFlags usage)
      -> std::shared_ptr<Buffer>;

  void set_data_raw(VmaAllocator allocator, size_t size, void* data);

  [[nodiscard]] auto get() const { return buffer; }

 private:
  VkBuffer buffer{};
  VmaAllocation allocation{};
};

}  // namespace wren
