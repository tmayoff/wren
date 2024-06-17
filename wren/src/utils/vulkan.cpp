#include "wren/utils/vulkan.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <tl/expected.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren_vk/errors.hpp>

// NOLINTBEGIN
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT =
    nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT
    pfnVkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkCmdPushConstants pfnVkCmdPushConstants = nullptr;
// NOLINTEND

VKAPI_ATTR auto VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
    VkAllocationCallbacks const *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) -> VkResult {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo,
                                           pAllocator, pMessenger);
}

VKAPI_ATTR auto VKAPI_CALL vkCmdPushDescriptorSetKHR(
    VkCommandBuffer buffer, VkPipelineLayout layout,
    VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
    void const *pValues) {
  pfnVkCmdPushConstants(buffer, layout, stageFlags, offset, size,
                        pValues);
}

namespace wren::vulkan {

// NOLINTNEXTLINE
#define LOAD_VULKAN_PFN(out, fn_name)                 \
  if (out == nullptr) {                               \
    out = reinterpret_cast<PFN_##fn_name>(            \
        instance.getProcAddr(#fn_name));              \
    if (out == nullptr)                               \
      return tl::make_unexpected(make_error_code(     \
          VK_NS::Result::eErrorExtensionNotPresent)); \
  }

auto LoadFunctions(VK_NS::Instance const &instance)
    -> expected<void> {
  LOAD_VULKAN_PFN(pfnVkCmdPushConstants, vkCmdPushConstants);
  LOAD_VULKAN_PFN(pfnVkCreateDebugUtilsMessengerEXT,
                  vkCreateDebugUtilsMessengerEXT);
  return {};
}

auto DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    VkDebugUtilsMessengerCallbackDataEXT const *msg_data,
    void *user_data) -> VkBool32 {
  switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      spdlog::trace("[{}] {}", msg_data->pMessageIdName,
                    msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      spdlog::info("[{}] {}", msg_data->pMessageIdName,
                   msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      spdlog::warn("[{}] {}", msg_data->pMessageIdName,
                   msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      spdlog::error("[{}] {}", msg_data->pMessageIdName,
                    msg_data->pMessage);
      break;
    default:
      spdlog::error("[{}] {}", msg_data->pMessageIdName,
                    msg_data->pMessage);
      break;
  }

  return VK_TRUE;
}

auto GetSwapchainSupportDetails(
    VK_NS::PhysicalDevice const &physical_device,
    VK_NS::SurfaceKHR const &surface)
    -> expected<SwapchainSupportDetails> {
  SwapchainSupportDetails details;

  VK_NS::Result res = VK_NS::Result::eSuccess;

  std::tie(res, details.surface_capabilites) =
      physical_device.getSurfaceCapabilitiesKHR(surface);
  if (res != VK_NS::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, details.surface_formats) =
      physical_device.getSurfaceFormatsKHR(surface);
  if (res != VK_NS::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::tie(res, details.present_modes) =
      physical_device.getSurfacePresentModesKHR(surface);
  if (res != VK_NS::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  return details;
}

}  // namespace wren::vulkan
