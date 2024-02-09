#include "wren/graphics_context.hpp"
#include "spdlog/spdlog.h"
#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include <algorithm>
#include <spdlog/fmt/ranges.h>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace wren {

auto GraphicsContext::Create(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers)
    -> tl::expected<GraphicsContext, std::error_code> {
  GraphicsContext graphics_context;

  {
    spdlog::debug("Creating instance...");
    auto res = graphics_context.CreateInstance(
        application_name, requested_extensions, requested_layers);
    if (!res.has_value()) {
      return tl::make_unexpected(res.error());
    }
    spdlog::debug("Created instance.");
  }

#ifdef WREN_DEBUG
  {
    spdlog::debug("Creating debug messenger...");
    auto res = graphics_context.CreateDebugMessenger();
    if (!res.has_value())
      return tl::make_unexpected(res.error());
    spdlog::debug("Created debug messenger.");
  }
#endif

  {
    spdlog::debug("Picking physical device...");
    auto res = graphics_context.PickPhysicalDevice();
    if (!res.has_value())
      return tl::make_unexpected(res.error());
    spdlog::debug("Picked physical device.");
  }

  return graphics_context;
}

void GraphicsContext::Shutdown() { instance.destroy(); }

auto GraphicsContext::CreateInstance(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers)
    -> tl::expected<void, std::error_code> {
  const auto appInfo = vk::ApplicationInfo(application_name.c_str(), 1, "wren",
                                           1, VK_API_VERSION_1_3);

  spdlog::debug("Requesting extensions:");
  std::vector<const char *> extensions;
  for (const auto &ext : requested_extensions) {
    spdlog::debug("\t{}", ext);
    if (vulkan::IsExtensionSupport(ext)) {
      extensions.push_back(ext.data());
    } else {
      spdlog::warn("Requested vulkan extension {}, is not supported", ext);
    }
  }

  std::vector<const char *> layers;

  spdlog::debug("Requesting layers:");
  for (const auto &layer : requested_layers) {
    spdlog::debug("\t{}", layer);
    if (vulkan::IsLayerSupported(layer)) {
      extensions.push_back(layer.data());
    } else {
      spdlog::warn("Requested vulkan layer {}, is not supported", layer);
    }
  }

#ifdef WREN_DEBUG
  if (vulkan::IsLayerSupported("VK_LAYER_KHRONOS_validation")) {
    spdlog::debug("Validation layer supported, adding to instance");
    layers.push_back("VK_LAYER_KHRONOS_validation");
  }

  if (vulkan::IsExtensionSupport(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
    spdlog::debug("Debug utils extension supported, adding to instance");
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

  vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
      // vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

  vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
      {}, severity_flags, message_type_flags, &vulkan::DebugCallback);
#endif

  vk::InstanceCreateInfo createInfo({}, &appInfo, layers, extensions);
#ifdef WREN_DEBUG
  createInfo.setPNext(&debugMessengerCreateInfo);
#endif

  auto [res, instance] = vk::createInstance(createInfo);
  if (res != vk::Result::eSuccess) {
    return tl::make_unexpected(make_error_code(res));
  }

  this->instance = instance;

  return {};
}

auto GraphicsContext::PickPhysicalDevice()
    -> tl::expected<void, std::error_code> {

  auto devices = instance.enumeratePhysicalDevices();

  for (const auto &device : devices.value) {
    if (IsDeviceSuitable(device)) {
      physical_device = device;
      break;
    }
  }

  if (!physical_device) {
    return tl::make_unexpected(make_error_code(VulkanErrors::NoDevicesFound));
  }

  return {};
}

auto GraphicsContext::IsDeviceSuitable(const vk::PhysicalDevice &device)
    -> bool {
  auto res = vulkan::Queue::FindQueueFamilyIndices(device);
  if (!res.has_value()) {
    spdlog::error("{}", res.error().message());
    return false;
  }

  return true;
}

auto GraphicsContext::CreateDevice() -> tl::expected<void, std::error_code> {
  auto res = vulkan::Device::Create(instance, physical_device, surface);
  if (!res.has_value())
    return tl::make_unexpected(res.error());
  device = res.value();

  return {};
}

#ifdef WREN_DEBUG
auto GraphicsContext::CreateDebugMessenger()
    -> tl::expected<void, std::error_code> {
  vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

  vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
      // vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

  vk::DebugUtilsMessengerCreateInfoEXT createInfo(
      {}, severity_flags, message_type_flags, &vulkan::DebugCallback);

  auto [res, debug_messenger] =
      instance.createDebugUtilsMessengerEXT(createInfo, nullptr);
  if (res != vk::Result::eSuccess) {
    return tl::unexpected(make_error_code(res));
  }

  this->debug_messenger = debug_messenger;

  return {};
}
#endif

} // namespace wren
