#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren/utils/result.hpp>

namespace wren::vulkan {

class Device {
 public:
  static auto create(const ::vk::Instance &instance,
                     const ::vk::PhysicalDevice &physical_device,
                     const ::vk::SurfaceKHR &surface) -> expected<Device>;

  [[nodiscard]] auto get() const -> ::vk::Device { return device_; }

  [[nodiscard]] auto get_graphics_queue() const { return graphics_queue_; }

  [[nodiscard]] auto get_present_queue() const { return present_queue_; }

  [[nodiscard]] auto command_pool() const { return command_pool_; }

 private:
  auto create_device(const ::vk::Instance &instance,
                     const ::vk::PhysicalDevice &physical_device,
                     const ::vk::SurfaceKHR &surface) -> expected<void>;

  ::vk::CommandPool command_pool_;
  ::vk::Device device_;
  ::vk::Queue graphics_queue_;
  ::vk::Queue present_queue_;
};

}  // namespace wren::vulkan
