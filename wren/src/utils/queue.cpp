#include "wren/utils/queue.hpp"

#include <optional>
#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <wren_vk/errors.hpp>

#include "vulkan/vulkan.hpp"
#include "vulkan/vulkan_handles.hpp"

namespace wren::vulkan {

auto Queue::FindQueueFamilyIndices(
    ::vk::PhysicalDevice const &physical_device,
    std::optional<::vk::SurfaceKHR> const &surface)
    -> expected<QueueFamilyIndices> {
  auto const &queue_families =
      physical_device.getQueueFamilyProperties();

  std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
  uint32_t i = 0;
  for (auto const &f : queue_families) {
    if (f.queueFlags & ::vk::QueueFlagBits::eGraphics)
      graphics_family = i;

    if (surface.has_value()) {
      auto res =
          physical_device.getSurfaceSupportKHR(i, surface.value());
      if (res.result != ::vk::Result::eSuccess)
        return tl::make_unexpected(make_error_code(res.result));

      if (res.value) {
        present_family = i;
      }
    }

    i++;
  }

  if (!graphics_family.has_value())
    return tl::make_unexpected(
        make_error_code(VulkanErrors::QueueFamilyNotSupported));

  if (surface.has_value())
    if (!present_family.has_value())
      return tl::make_unexpected(
          make_error_code(VulkanErrors::QueueFamilyNotSupported));

  return QueueFamilyIndices{graphics_family.value()};
}

}  // namespace wren::vulkan
