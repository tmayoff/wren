#include "wren/utils/device.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren/vk/result.hpp>

#include "wren/utils/queue.hpp"

namespace wren::vulkan {

auto Device::create(const ::vk::Instance &instance,
                    const ::vk::PhysicalDevice &physical_device,
                    const ::vk::SurfaceKHR &surface) -> expected<Device> {
  Device device;

  auto res = device.create_device(instance, physical_device, surface);
  if (!res.has_value()) return std::unexpected(res.error());

  return device;
}

auto Device::create_device(const ::vk::Instance &instance,
                           const ::vk::PhysicalDevice &physical_device,
                           const ::vk::SurfaceKHR &surface) -> expected<void> {
  const auto indices =
      Queue::find_queue_family_indices(physical_device, surface);

  float queue_prio = 0.0f;
  ::vk::DeviceQueueCreateInfo queue_create_info({}, indices->graphics_index, 1,
                                                &queue_prio);
  std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                           VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};

  {
    auto features2 =
        physical_device
            .getFeatures2< ::vk::PhysicalDeviceFeatures2,
                           ::vk::PhysicalDeviceImagelessFramebufferFeatures>();

    ::vk::DeviceCreateInfo create_info({}, queue_create_info, {}, extensions,
                                       {}, &features2);
    auto res = physical_device.createDevice(create_info);
    if (res.result != ::vk::Result::eSuccess)
      return std::unexpected(make_error_code(res.result));
    device_ = res.value;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device_);
  }

  graphics_queue_ = device_.getQueue(indices->graphics_index, 0);
  present_queue_ = device_.getQueue(indices->present_index, 0);

  {
    const ::vk::CommandPoolCreateInfo create_info({}, indices->graphics_index);
    VK_TIE_ERR_PROP(command_pool_, device_.createCommandPool(create_info));
  }

  return {};
}

}  // namespace wren::vulkan
