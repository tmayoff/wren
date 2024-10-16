#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren/utils/errors.hpp>

namespace wren::vulkan {

class Device {
 public:
  static auto create(::vk::Instance const &instance,
                     ::vk::PhysicalDevice const &physical_device,
                     ::vk::SurfaceKHR const &surface)
      -> expected<Device>;

  [[nodiscard]] auto get() const -> ::vk::Device { return device_; }

  [[nodiscard]] auto get_graphics_queue() const {
    return graphics_queue_;
  }

  [[nodiscard]] auto get_present_queue() const {
    return present_queue_;
  }

  [[nodiscard]] auto command_pool() const { return command_pool_; }

 private:
  auto create_device(::vk::Instance const &instance,
                    ::vk::PhysicalDevice const &physical_device,
                    ::vk::SurfaceKHR const &surface)
      -> expected<void>;

  ::vk::CommandPool command_pool_;
  ::vk::Device device_;
  ::vk::Queue graphics_queue_;
  ::vk::Queue present_queue_;
};

}  // namespace wren::vulkan
