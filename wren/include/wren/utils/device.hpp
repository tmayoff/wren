#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_utils/errors.hpp>

namespace wren::vulkan {

class Device {
 public:
  static auto Create(::vk::Instance const &instance,
                     ::vk::PhysicalDevice const &physical_device,
                     ::vk::SurfaceKHR const &surface)
      -> expected<Device>;

  [[nodiscard]] auto get() const -> ::vk::Device { return device; }

  [[nodiscard]] auto get_graphics_queue() const {
    return graphics_queue;
  }

  [[nodiscard]] auto get_present_queue() const {
    return present_queue;
  }

  [[nodiscard]] auto command_pool() const { return command_pool_; }

 private:
  auto CreateDevice(::vk::Instance const &instance,
                    ::vk::PhysicalDevice const &physical_device,
                    ::vk::SurfaceKHR const &surface)
      -> expected<void>;

  ::vk::CommandPool command_pool_;
  ::vk::Device device;
  ::vk::Queue graphics_queue;
  ::vk::Queue present_queue;
};

}  // namespace wren::vulkan
