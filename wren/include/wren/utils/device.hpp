#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren::vulkan {

class Device {
public:
  static auto Create(const vk::Instance &instance,
                     const vk::PhysicalDevice &physical_device,
                     const vk::SurfaceKHR &surface)
      -> tl::expected<Device, std::error_code>;

  [[nodiscard]] auto get() const -> vk::Device { return device; }

private:
  auto CreateDevice(const vk::Instance &instance,
                    const vk::PhysicalDevice &physical_device,
                    const vk::SurfaceKHR &surface)
      -> tl::expected<void, std::error_code>;

  vk::Device device;
  vk::Queue graphics_queue;
  vk::Queue present_queue;
};

} // namespace wren::vulkan
