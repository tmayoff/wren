#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren::vulkan {

auto Queue::FindQueueFamilyIndices(const vk::PhysicalDevice &physical_device)
    -> tl::expected<QueueFamilyIndices, std::error_code> {
  const auto &queue_families = physical_device.getQueueFamilyProperties();

  std::optional<uint32_t> graphics_family;
  uint32_t i = 0;
  for (const auto &f : queue_families) {
    if (f.queueFlags & vk::QueueFlagBits::eGraphics) {
      graphics_family = i;
    }

    i++;
  }

  if (!graphics_family.has_value())
    return tl::make_unexpected(
        make_error_code(VulkanErrors::QueueFamilyNotSupported));

  return QueueFamilyIndices{graphics_family.value()};
}

} // namespace wren::vulkan
