#pragma once

#include <spdlog/spdlog.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren/utils/errors.hpp>

// VKAPI_ATTR auto VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
//     VkInstance instance,
//     VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
//     VkAllocationCallbacks const *pAllocator,
//     VkDebugUtilsMessengerEXT *pMessenger) -> VkResult;
//
// VKAPI_ATTR auto VKAPI_CALL vkCmdPushDescriptorSetKHR(
//     VkCommandBuffer buffer, VkPipelineLayout layout,
//     VkShaderStageFlags stageFlags, uint32_t offset, uint32_t size,
//     void const *pValues);

namespace wren::vulkan {
struct SwapchainSupportDetails {
  ::vk::SurfaceCapabilitiesKHR surface_capabilites;
  std::vector<::vk::SurfaceFormatKHR> surface_formats;
  std::vector<::vk::PresentModeKHR> present_modes;
};

auto load_functions(const ::vk::Instance &instance) -> expected<void>;

auto get_swapchain_support_details(const ::vk::PhysicalDevice &physical_device,
                                   const ::vk::SurfaceKHR &surface)
    -> expected<SwapchainSupportDetails>;

VKAPI_ATTR auto VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
               void *p_user_data) -> VkBool32;

inline auto is_extension_supported(const std::string_view &name) -> bool {
  const auto res = ::vk::enumerateInstanceExtensionProperties();

  bool found = false;
  for (const auto &ext : res.value) {
    if (std::strcmp(ext.extensionName.data(), name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

inline auto is_layer_supported(const std::string_view &name) -> bool {
  const auto res = ::vk::enumerateInstanceLayerProperties();

  bool found = false;
  for (const auto &ext : res.value) {
    if (std::strcmp(ext.layerName, name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

inline auto is_device_extension_supported(const std::string_view &name,
                                          const ::vk::PhysicalDevice &device) {
  const auto res = device.enumerateDeviceExtensionProperties();

  bool found = false;
  for (const auto &ext : res.value) {
    if (std::strcmp(ext.extensionName, name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

}  // namespace wren::vulkan
