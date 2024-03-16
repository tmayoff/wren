#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren::vulkan {

class Device {
 public:
  static auto Create(vk::Instance const &instance,
                     vk::PhysicalDevice const &physical_device,
                     vk::SurfaceKHR const &surface)
      -> tl::expected<Device, std::error_code>;

  [[nodiscard]] auto get() const -> vk::Device { return device; }

  [[nodiscard]] auto get_graphics_queue() const {
    return graphics_queue;
  }

  [[nodiscard]] auto get_present_queue() const {
    return present_queue;
  }

  [[nodiscard]] auto command_pool() const { return command_pool_; }

 private:
  auto CreateDevice(vk::Instance const &instance,
                    vk::PhysicalDevice const &physical_device,
                    vk::SurfaceKHR const &surface)
      -> tl::expected<void, std::error_code>;

  vk::CommandPool command_pool_;
  vk::Device device;
  vk::Queue graphics_queue;
  vk::Queue present_queue;
};

}  // namespace wren::vulkan
