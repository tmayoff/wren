#include "wren/utils/device.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_vk/errors.hpp>

#include "wren/utils/queue.hpp"

namespace wren::vulkan {

auto Device::Create(::vk::Instance const &instance,
                    ::vk::PhysicalDevice const &physical_device,
                    ::vk::SurfaceKHR const &surface)
    -> expected<Device> {
  Device device;

  auto res = device.CreateDevice(instance, physical_device, surface);
  if (!res.has_value()) return tl::make_unexpected(res.error());

  return device;
}

auto Device::CreateDevice(
    ::vk::Instance const &instance,
    ::vk::PhysicalDevice const &physical_device,
    ::vk::SurfaceKHR const &surface)
    -> expected<void> {
  auto const indices =
      Queue::FindQueueFamilyIndices(physical_device, surface);

  float queue_prio = 0.0f;
  ::vk::DeviceQueueCreateInfo queue_create_info(
      {}, indices->graphics_index, 1, &queue_prio);
  std::array extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                           VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME};

  {
    auto features2 = physical_device.getFeatures2<
        ::vk::PhysicalDeviceFeatures2,
        ::vk::PhysicalDeviceImagelessFramebufferFeatures>();

    ::vk::DeviceCreateInfo createInfo({}, queue_create_info, {},
                                       extensions, {}, &features2);
    auto res = physical_device.createDevice(createInfo);
    if (res.result != ::vk::Result::eSuccess)
      return tl::make_unexpected(make_error_code(res.result));
    device = res.value;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
  }

  graphics_queue = device.getQueue(indices->graphics_index, 0);
  present_queue = device.getQueue(indices->present_index, 0);

  {
    ::vk::CommandPoolCreateInfo const create_info(
        {}, indices->graphics_index);
    VK_TIE_ERR_PROP(command_pool_,
                    device.createCommandPool(create_info));
  }

  return {};
}

}  // namespace wren::vulkan
