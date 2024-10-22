#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <span>
#include <vulkan/vulkan.hpp>
#include <wren/vk/errors.hpp>

namespace wren::vk {

class Buffer {
 public:
  static auto create(const VmaAllocator &allocator, size_t size,
                     VkBufferUsageFlags usage,
                     const std::optional<VmaAllocationCreateFlags> &flags = {})
      -> std::shared_ptr<Buffer>;

  static auto copy_buffer(const ::vk::Device &device,
                          const ::vk::Queue &submit_queue,
                          const ::vk::CommandPool &command_pool,
                          const std::shared_ptr<Buffer> &src,
                          const std::shared_ptr<Buffer> &dst, size_t size)
      -> expected<void>;

  Buffer(const VmaAllocator &allocator) : allocator_(allocator) {}
  ~Buffer();

  Buffer(const Buffer &) = delete;
  Buffer(Buffer &&) = delete;
  auto operator=(const Buffer &) = delete;
  auto operator=(Buffer &&) = delete;

  template <typename T>
  auto set_data_raw(std::span<const T> data) -> expected<void>;

  auto set_data_raw(const void *data, std::size_t size) -> expected<void>;

  auto map() {
    vmaMapMemory(allocator_, allocation_, &mapped_ptr_);
    return mapped_ptr_;
  }

  auto unmap() {
    if (mapped_ptr_ == nullptr) return;
    vmaUnmapMemory(allocator_, allocation_);
    mapped_ptr_ = nullptr;
  }

  [[nodiscard]] auto get() const { return buffer_; }

 private:
  ::vk::Buffer buffer_{};
  VmaAllocator allocator_ = nullptr;
  VmaAllocation allocation_{};

  void *mapped_ptr_ = nullptr;
};

template <typename T>
auto Buffer::set_data_raw(std::span<const T> data) -> expected<void> {
  VK_ERR_PROP_VOID(static_cast<::vk::Result>(vmaCopyMemoryToAllocation(
      allocator_, data.data(), allocation_, {0}, {data.size_bytes()})));
  return {};
}

}  // namespace wren::vk
