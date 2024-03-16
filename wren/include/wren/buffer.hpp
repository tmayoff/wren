#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <span>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

#include "utils/vulkan_errors.hpp"

namespace wren {

class GraphicsContext;

class Buffer {
 public:
  static auto Create(VmaAllocator const& allocator, size_t size,
                     VkBufferUsageFlags usage)
      -> std::shared_ptr<Buffer>;

  template <typename T>
  auto set_data_raw(VmaAllocator allocator, std::span<T const> data)
      -> tl::expected<void, std::error_code>;

  [[nodiscard]] auto get() const { return buffer; }

 private:
  vk::Buffer buffer{};
  VmaAllocation allocation{};
};

template <typename T>
auto Buffer::set_data_raw(VmaAllocator allocator,
                          std::span<T const> data)
    -> tl::expected<void, std::error_code> {
  void* mapped = nullptr;

  vmaInvalidateAllocation(allocator, allocation, 0,
                          data.size_bytes());

  VK_ERR_PROP_VOID(static_cast<vk::Result>(
      vmaMapMemory(allocator, allocation, &mapped)));
  std::memcpy(mapped, data.data(), data.size_bytes());
  vmaUnmapMemory(allocator, allocation);

  vmaFlushAllocation(allocator, allocation, 0, data.size_bytes());
  return {};
}

}  // namespace wren
