#pragma once

#include <cstdint>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_utils/errors.hpp>

namespace wren::vulkan {

struct QueueFamilyIndices {
  uint32_t graphics_index;
  uint32_t present_index;
};

class Queue {
 public:
  static auto FindQueueFamilyIndices(
      const VK_NS::PhysicalDevice &physical_device,
      std::optional<VK_NS::SurfaceKHR> const &surface = {})
      -> expected<QueueFamilyIndices>;
};

}  // namespace wren::vulkan
