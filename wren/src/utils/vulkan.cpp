#include "wren/utils/vulkan.hpp"
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_to_string.hpp>

// NOLINTBEGIN
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;
// NOLINTEND

VKAPI_ATTR auto VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) -> VkResult {
  if (pfnVkCreateDebugUtilsMessengerEXT == nullptr) {
    // NOLINTBEGIN
    pfnVkCreateDebugUtilsMessengerEXT =
        reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    // NOLINTEND
  }

  if (pfnVkCreateDebugUtilsMessengerEXT == nullptr) {
    return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator,
                                           pMessenger);

  return VkResult::VK_ERROR_EXTENSION_NOT_PRESENT;
}

namespace wren::vulkan {

auto DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                   VkDebugUtilsMessageTypeFlagsEXT type,
                   const VkDebugUtilsMessengerCallbackDataEXT *msg_data,
                   void *user_data) -> VkBool32 {

  std::string msg =
      fmt::format("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);

  switch (severity) {
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
    spdlog::trace(msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
    spdlog::info(msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
    spdlog::warn(msg);
    break;
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
  case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
    spdlog::error(msg);
    break;
  }

  return VK_TRUE;
}

auto GetSwapchainSupportDetails(const vk::PhysicalDevice &physical_device,
                                const vk::SurfaceKHR &surface)
    -> tl::expected<SwapchainSupportDetails, std::error_code> {
  SwapchainSupportDetails details;

  vk::Result res = vk::Result::eSuccess;

  std::tie(res, details.surface_capabilites) =
      physical_device.getSurfaceCapabilitiesKHR(surface);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, details.surface_formats) =
      physical_device.getSurfaceFormatsKHR(surface);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, details.present_modes) =
      physical_device.getSurfacePresentModesKHR(surface);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  return details;
}

} // namespace wren::vulkan
