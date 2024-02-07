#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

namespace wren::vulkan {

class Device {
public:
  static auto Create(const vk::Instance &instance,
                     const vk::PhysicalDevice &physical_device)
      -> tl::expected<Device, std::error_code>;
};

} // namespace wren::vulkan
