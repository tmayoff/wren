#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <span>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <wren_vk/errors.hpp>

namespace wren::vk {

class Buffer {
 public:
  static auto Create(
      VmaAllocator const& allocator, size_t size,
      VkBufferUsageFlags usage,
      std::optional<VmaAllocationCreateFlags> const& flags = {})
      -> std::shared_ptr<Buffer>;

  static auto copy_buffer(VK_NS::Device const& device,
                          VK_NS::Queue const& submit_queue,
                          VK_NS::CommandPool const& command_pool,
                          std::shared_ptr<Buffer> const& src,
                          std::shared_ptr<Buffer> const& dst,
                          size_t size) -> expected<void>;

  Buffer(VmaAllocator const& allocator) : allocator(allocator) {}
  ~Buffer();

  Buffer(Buffer const&) = delete;
  Buffer(Buffer&&) = delete;
  auto operator=(Buffer const&) = delete;
  auto operator=(Buffer&&) = delete;

  template <typename T>
  auto set_data_raw(std::span<T const> data) -> expected<void>;

  auto set_data_raw(void const* data, std::size_t size)
      -> expected<void>;

  [[nodiscard]] auto get() const { return buffer; }

 private:
  _VK_::Buffer buffer{};
  VmaAllocator allocator = nullptr;
  VmaAllocation allocation{};
};

template <typename T>
auto Buffer::set_data_raw(std::span<T const> data) -> expected<void> {
  VK_ERR_PROP_VOID(static_cast<VK_NS::Result>(
      vmaCopyMemoryToAllocation(allocator, data.data(), allocation,
                                {0}, {data.size_bytes()})));
  return {};
}

}  // namespace wren::vk
