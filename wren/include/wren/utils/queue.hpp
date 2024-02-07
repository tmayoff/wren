#pragma once

#include <cstdint>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren::vulkan {

struct QueueFamilyIndices {
  uint32_t graphics_index;
};

class Queue {
public:
  static auto FindQueueFamilyIndices(const vk::PhysicalDevice &physical_device)
      -> tl::expected<QueueFamilyIndices, std::error_code>;
};

} // namespace wren::vulkan
