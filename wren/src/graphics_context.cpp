#include "graphics_context.hpp"

#include <spdlog/fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren/utils/errors.hpp>
#include <wren/vk/errors.hpp>

#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan.hpp"

namespace wren {

auto GraphicsContext::Create(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers)
    -> expected<std::shared_ptr<GraphicsContext>> {
  auto graphics_context =
      std::shared_ptr<GraphicsContext>(new GraphicsContext());

  VULKAN_HPP_DEFAULT_DISPATCHER.init();

  {
    spdlog::debug("Creating instance...");
    TRY_RESULT(graphics_context->CreateInstance(
        application_name, requested_extensions, requested_layers));
    spdlog::debug("Created instance.");
  }

#ifdef WREN_DEBUG
  {
    spdlog::debug("Creating debug messenger...");
    // auto res = graphics_context->CreateDebugMessenger();
    // if (!res.has_value()) return std::unexpected(res.error());
    spdlog::debug("Created debug messenger.");
  }
#endif

  return graphics_context;
}

GraphicsContext::~GraphicsContext() { instance.destroy(); }

auto GraphicsContext::CreateInstance(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers) -> expected<void> {
  const auto appInfo = ::vk::ApplicationInfo(application_name.c_str(), 1,
                                             "wren", 1, VK_API_VERSION_1_2);

  spdlog::debug("Requesting extensions:");
  std::vector<const char *> extensions = {
      VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME};
  for (const auto &ext : requested_extensions) {
    spdlog::debug("\t{}", ext);
    if (vulkan::is_extension_supported(ext)) {
      extensions.push_back(ext.data());
    } else {
      spdlog::warn("Requested vulkan extension {}, is not supported", ext);
    }
  }

  std::vector<const char *> layers;

  spdlog::debug("Requesting layers:");
  for (const auto &layer : requested_layers) {
    spdlog::debug("\t{}", layer);
    if (vulkan::is_layer_supported(layer)) {
      extensions.push_back(layer.data());
    } else {
      spdlog::warn("Requested vulkan layer {}, is not supported", layer);
    }
  }

#ifdef WREN_DEBUG
  if (vulkan::is_layer_supported("VK_LAYER_KHRONOS_validation")) {
    if (getenv("ENABLE_VULKAN_RENDERDOC_CAPTURE") == nullptr) {
      spdlog::debug("Validation layer supported, adding to instance");
      layers.push_back("VK_LAYER_KHRONOS_validation");
    } else {
      spdlog::info("Renderdoc enabled can't enable validation layers");
    }
  }

  if (vulkan::is_extension_supported(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
    spdlog::debug("Debug utils extension supported, adding to instance");
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  ::vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
      // ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

  ::vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
      // ::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
      ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

  ::vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
      {}, severity_flags, message_type_flags, &vulkan::debug_callback);
#endif

  ::vk::InstanceCreateInfo createInfo({}, &appInfo, layers, extensions);
#ifdef WREN_DEBUG
  createInfo.setPNext(&debugMessengerCreateInfo);
#endif

  VK_TIE_ERR_PROP(this->instance, ::vk::createInstance(createInfo));

  VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);

  return {};
}

auto GraphicsContext::SetupDevice() -> expected<void> {
  {
    spdlog::debug("Picking physical device...");
    TRY_RESULT(PickPhysicalDevice());
    spdlog::debug("Picked physical device.");
  }

  {
    spdlog::debug("Creating logical device...");
    TRY_RESULT(CreateDevice());
    spdlog::debug("Created logical device.");
  }

  VmaVulkanFunctions vma_functions{};
  vma_functions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
  vma_functions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

  VmaAllocatorCreateInfo create_info{};
  create_info.vulkanApiVersion = VK_API_VERSION_1_2;
  create_info.physicalDevice = physical_device;
  create_info.device = device.get();
  create_info.instance = instance;
  create_info.pVulkanFunctions = &vma_functions;
  vmaCreateAllocator(&create_info, &allocator_);

  return {};
}

auto GraphicsContext::PickPhysicalDevice() -> expected<void> {
  auto devices = instance.enumeratePhysicalDevices();

  for (const auto &device : devices.value) {
    if (IsDeviceSuitable(device)) {
      physical_device = device;
      break;
    }
  }

  if (!physical_device) {
    return std::unexpected(make_error_code(VulkanErrors::NoDevicesFound));
  }

  return {};
}

auto GraphicsContext::IsDeviceSuitable(const ::vk::PhysicalDevice &device)
    -> bool {
  auto res = vulkan::Queue::find_queue_family_indices(device);
  if (!res.has_value()) {
    spdlog::error("{}", res.error().message());
    return false;
  }

  bool swapchain_support = vulkan::is_device_extension_supported(
      VK_KHR_SWAPCHAIN_EXTENSION_NAME, device);
  if (!swapchain_support) return false;

  auto swapchain_details =
      vulkan::get_swapchain_support_details(device, surface);
  if (!swapchain_details.has_value()) {
    return false;
  }

  if (swapchain_details->present_modes.empty() ||
      swapchain_details->surface_formats.empty())
    return false;

  return true;
}

auto GraphicsContext::CreateDevice() -> expected<void> {
  TRY_RESULT(device,
             vulkan::Device::create(instance, physical_device, surface));
  return {};
}

#ifdef WREN_DEBUG
auto GraphicsContext::CreateDebugMessenger() -> expected<void> {
  ::vk::DebugUtilsMessageSeverityFlagsEXT severity_flags(
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

  ::vk::DebugUtilsMessageTypeFlagsEXT message_type_flags(
      // ::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
      ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance);

  ::vk::DebugUtilsMessengerCreateInfoEXT createInfo(
      {}, severity_flags, message_type_flags, &vulkan::debug_callback);

  VK_TIE_ERR_PROP(this->debug_messenger,
                  instance.createDebugUtilsMessengerEXT(createInfo, nullptr));

  return {};
}
#endif

}  // namespace wren
