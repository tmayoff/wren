#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <span>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

#include "utils/device.hpp"
#include "utils/vulkan_errors.hpp"

namespace wren {

class GraphicsContext;

class Buffer {
 public:
  static auto Create(
      VmaAllocator const& allocator, size_t size,
      VkBufferUsageFlags usage,
      std::optional<VmaAllocationCreateFlags> const& flags = {})
      -> std::shared_ptr<Buffer>;

  static auto copy_buffer(vulkan::Device const& device,
                          vk::CommandPool const& command_pool,
                          std::shared_ptr<Buffer> const& src,
                          std::shared_ptr<Buffer> const& dst,
                          size_t size)
      -> tl::expected<void, std::error_code>;

  Buffer(VmaAllocator const& allocator) : allocator(allocator) {}
  ~Buffer();

  Buffer(Buffer const&) = delete;
  Buffer(Buffer&&) = delete;
  auto operator=(Buffer const&) = delete;
  auto operator=(Buffer&&) = delete;

  template <typename T>
  auto set_data_raw(VmaAllocator allocator, std::span<T const> data)
      -> tl::expected<void, std::error_code>;

  [[nodiscard]] auto get() const { return buffer; }

 private:
  vk::Buffer buffer{};
  VmaAllocator allocator;
  VmaAllocation allocation{};
};

template <typename T>
auto Buffer::set_data_raw(VmaAllocator allocator,
                          std::span<T const> data)
    -> tl::expected<void, std::error_code> {
  VK_ERR_PROP_VOID(static_cast<vk::Result>(vmaCopyMemoryToAllocation(
      allocator, data.data(), allocation, {0}, {data.size_bytes()})));
  return {};
}

}  // namespace wren
