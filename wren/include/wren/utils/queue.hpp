#pragma once

#include <cstdint>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren/utils/result.hpp>

namespace wren::vulkan {

struct QueueFamilyIndices {
  uint32_t graphics_index;
  uint32_t present_index;
};

class Queue {
 public:
  static auto find_queue_family_indices(
      const ::vk::PhysicalDevice &physical_device,
      const std::optional<::vk::SurfaceKHR> &surface = {})
      -> expected<QueueFamilyIndices>;
};

}  // namespace wren::vulkan
