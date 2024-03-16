#include "buffer.hpp"

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "utils/vulkan_errors.hpp"

namespace wren {

auto Buffer::Create(
    VmaAllocator const& allocator, size_t size,
    VkBufferUsageFlags usage,
    std::optional<VmaAllocationCreateFlags> const& flags)
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
  vmaCreateBuffer(allocator, &create_info, &alloc_info, &buf,
                  &b->allocation, nullptr);
  b->buffer = buf;

  return b;
}

auto Buffer::copy_buffer(vulkan::Device const& device,
                         vk::CommandPool const& command_pool,
                         std::shared_ptr<Buffer> const& src,
                         std::shared_ptr<Buffer> const& dst,
                         size_t size)
    -> tl::expected<void, std::error_code> {
  vk::CommandBufferAllocateInfo const alloc_info(
      command_pool, vk::CommandBufferLevel::ePrimary, 1);

  VK_ERR_PROP(cmd_bufs,
              device.get().allocateCommandBuffers(alloc_info));

  auto const cmd_buf = cmd_bufs.front();

  vk::CommandBufferBeginInfo begin_info(
      vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

  VK_ERR_PROP_VOID(cmd_buf.begin(begin_info));

  vk::BufferCopy copy_region(0, 0, size);
  cmd_buf.copyBuffer(src->get(), dst->get(), copy_region);
  VK_ERR_PROP_VOID(cmd_buf.end());

  vk::SubmitInfo submit_info({}, {}, cmd_buf);
  VK_ERR_PROP_VOID(device.get_graphics_queue().submit(submit_info));
  VK_ERR_PROP_VOID(device.get_graphics_queue().waitIdle());

  device.get().freeCommandBuffers(device.command_pool(), cmd_bufs);

  return {};
}

Buffer::~Buffer() { vmaFreeMemory(allocator, allocation); }

}  // namespace wren
