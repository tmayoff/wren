#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_utils/errors.hpp>

namespace wren::vulkan {

class Device {
 public:
  static auto Create(VK_NS::Instance const &instance,
                     VK_NS::PhysicalDevice const &physical_device,
                     VK_NS::SurfaceKHR const &surface)
      -> expected<Device>;

  [[nodiscard]] auto get() const -> VK_NS::Device { return device; }

  [[nodiscard]] auto get_graphics_queue() const {
    return graphics_queue;
  }

  [[nodiscard]] auto get_present_queue() const {
    return present_queue;
  }

  [[nodiscard]] auto command_pool() const { return command_pool_; }

 private:
  auto CreateDevice(VK_NS::Instance const &instance,
                    VK_NS::PhysicalDevice const &physical_device,
                    VK_NS::SurfaceKHR const &surface)
      -> expected<void>;

  VK_NS::CommandPool command_pool_;
  VK_NS::Device device;
  VK_NS::Queue graphics_queue;
  VK_NS::Queue present_queue;
};

}  // namespace wren::vulkan
