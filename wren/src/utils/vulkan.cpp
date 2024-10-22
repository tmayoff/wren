#include "wren/utils/vulkan.hpp"

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan_to_string.hpp>
#include <wren/vk/errors.hpp>

// NOLINTBEGIN
PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT =
    nullptr;
PFN_vkCmdPushConstants pfnVkCmdPushConstants = nullptr;
// NOLINTEND

VKAPI_ATTR auto VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
    const VkAllocationCallbacks *p_allocator,
    VkDebugUtilsMessengerEXT *p_messenger) -> VkResult {
  return pfnVkCreateDebugUtilsMessengerEXT(instance, p_create_info, p_allocator,
                                           p_messenger);
}

VKAPI_ATTR auto VKAPI_CALL
vkCmdPushDescriptorSetKHR(VkCommandBuffer buffer, VkPipelineLayout layout,
                          VkShaderStageFlags stage_flags, uint32_t offset,
                          uint32_t size, const void *p_values) {
  pfnVkCmdPushConstants(buffer, layout, stage_flags, offset, size, p_values);
}

namespace wren::vulkan {

// NOLINTNEXTLINE
#define LOAD_VULKAN_PFN(out, fn_name)                                      \
  if (out == nullptr) {                                                    \
    out = reinterpret_cast<PFN_##fn_name>(instance.getProcAddr(#fn_name)); \
    if (out == nullptr)                                                    \
      return std::unexpected(                                              \
          make_error_code(::vk::Result::eErrorExtensionNotPresent));       \
  }

auto load_functions(const ::vk::Instance &instance) -> expected<void> {
  LOAD_VULKAN_PFN(pfnVkCmdPushConstants, vkCmdPushConstants);
  LOAD_VULKAN_PFN(pfnVkCreateDebugUtilsMessengerEXT,
                  vkCreateDebugUtilsMessengerEXT);
  return {};
}

auto debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                    VkDebugUtilsMessageTypeFlagsEXT type,
                    const VkDebugUtilsMessengerCallbackDataEXT *msg_data,
                    void *user_data) -> VkBool32 {
  switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      spdlog::trace("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      spdlog::info("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      spdlog::warn("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
      spdlog::error("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);
      break;
    default:
      spdlog::error("[{}] {}", msg_data->pMessageIdName, msg_data->pMessage);
      break;
  }

  return VK_TRUE;
}

auto get_swapchain_support_details(const ::vk::PhysicalDevice &physical_device,
                                   const ::vk::SurfaceKHR &surface)
    -> expected<SwapchainSupportDetails> {
  SwapchainSupportDetails details;

  ::vk::Result res = ::vk::Result::eSuccess;

  std::tie(res, details.surface_capabilites) =
      physical_device.getSurfaceCapabilitiesKHR(surface);
  if (res != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(res));

  std::tie(res, details.surface_formats) =
      physical_device.getSurfaceFormatsKHR(surface);
  if (res != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(res));

  std::tie(res, details.present_modes) =
      physical_device.getSurfacePresentModesKHR(surface);
  if (res != ::vk::Result::eSuccess)
    return std::unexpected(make_error_code(res));

  return details;
}

}  // namespace wren::vulkan
