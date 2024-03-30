#pragma once

#include <cstdint>
#include <optional>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

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
      -> tl::expected<QueueFamilyIndices, std::error_code>;
};

}  // namespace wren::vulkan
