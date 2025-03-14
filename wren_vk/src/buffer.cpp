#include "buffer.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace wren::vk {

auto Buffer::create(const VmaAllocator& allocator, size_t size,
                    VkBufferUsageFlags usage,
                    const std::optional<VmaAllocationCreateFlags>& flags)
    -> std::shared_ptr<Buffer> {
  auto b = std::make_shared<Buffer>(allocator);

  VkBufferCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  create_info.size = size;
  create_info.usage = static_cast<VkBufferUsageFlags>(usage);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
  if (flags) alloc_info.flags = *flags;

  VkBuffer buf{};
  vmaCreateBuffer(allocator, &create_info, &alloc_info, &buf, &b->allocation_,
                  nullptr);
  b->buffer_ = buf;

  return b;
}

auto Buffer::set_data_raw(const void* data, std::size_t size)
    -> expected<void> {
  if (data == nullptr) {
    return {};
  }

  const auto res = static_cast<::vk::Result>(
      vmaCopyMemoryToAllocation(allocator_, data, allocation_, 0, size));
  if (res != ::vk::Result::eSuccess) {
    throw std::runtime_error(utils::enum_to_string(res));
  }

  // VK_ERR_PROP_VOID(
  //     static_cast<::::vk::Result>(vmaCopyMemoryToAllocation(
  //         allocator, data, allocation, 0, size)));
  return {};
}

auto Buffer::copy_buffer(const ::vk::Device& device,
                         const ::vk::Queue& submit_queue,
                         const ::vk::CommandPool& command_pool,
                         const std::shared_ptr<Buffer>& src,
                         const std::shared_ptr<Buffer>& dst, size_t size)
    -> expected<void> {
  const ::vk::CommandBufferAllocateInfo alloc_info(
      command_pool, ::vk::CommandBufferLevel::ePrimary, 1);

  VK_ERR_PROP(cmd_bufs, device.allocateCommandBuffers(alloc_info));

  const auto cmd_buf = cmd_bufs.front();

  ::vk::CommandBufferBeginInfo begin_info(
      ::vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  VK_ERR_PROP_VOID(cmd_buf.begin(begin_info));

  ::vk::BufferCopy copy_region(0, 0, size);
  cmd_buf.copyBuffer(src->get(), dst->get(), copy_region);
  VK_ERR_PROP_VOID(cmd_buf.end());

  ::vk::SubmitInfo submit_info({}, {}, cmd_buf);
  VK_ERR_PROP_VOID(submit_queue.submit(submit_info));
  VK_ERR_PROP_VOID(submit_queue.waitIdle());

  device.freeCommandBuffers(command_pool, cmd_bufs);

  return {};
}

Buffer::~Buffer() {
  unmap();
  vmaFreeMemory(allocator_, allocation_);
}

}  // namespace wren::vk
