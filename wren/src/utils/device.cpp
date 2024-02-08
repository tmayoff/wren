#include "wren/utils/device.hpp"
#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace wren::vulkan {

auto Device::Create(const vk::Instance &instance,
                    const vk::PhysicalDevice &physical_device,
                    const vk::SurfaceKHR &surface)
    -> tl::expected<Device, std::error_code> {
  Device device;

  auto res = device.CreateDevice(instance, physical_device, surface);
  if (!res.has_value()) {
    return tl::make_unexpected(res.error());
  }

  return device;
}

auto Device::CreateDevice(const vk::Instance &instance,
                          const vk::PhysicalDevice &physical_device,
                          const vk::SurfaceKHR &surface)
    -> tl::expected<void, std::error_code> {

  const auto indices = Queue::FindQueueFamilyIndices(physical_device, surface);

  float queue_prio = 0.0f;
  vk::DeviceQueueCreateInfo queue_create_info({}, indices->graphics_index, 1,
                                              &queue_prio);
  {
    vk::DeviceCreateInfo createInfo({}, queue_create_info);
    auto res = physical_device.createDevice(createInfo);
    if (res.result != vk::Result::eSuccess) {
      return tl::make_unexpected(make_error_code(res.result));
    }
    device = res.value;
  }

  graphics_queue = device.getQueue(indices->graphics_index, 0);
  present_queue = device.getQueue(indices->present_index, 0);

  return {};
}

} // namespace wren::vulkan
