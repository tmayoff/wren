#pragma once

#include <spdlog/spdlog.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

#include <tl/expected.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_utils/errors.hpp>

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
  VK_NS::SurfaceCapabilitiesKHR surface_capabilites;
  std::vector<VK_NS::SurfaceFormatKHR> surface_formats;
  std::vector<VK_NS::PresentModeKHR> present_modes;
};

auto LoadFunctions(VK_NS::Instance const &instance) -> expected<void>;

auto GetSwapchainSupportDetails(
    VK_NS::PhysicalDevice const &physical_device,
    VK_NS::SurfaceKHR const &surface)
    -> expected<SwapchainSupportDetails>;

VKAPI_ATTR auto VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData) -> VkBool32;

inline auto IsExtensionSupport(std::string_view const &name) -> bool {
  auto res = VK_NS::enumerateInstanceExtensionProperties();

  bool found = false;
  for (auto const &ext : res.value) {
    if (std::strcmp(ext.extensionName.data(), name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

inline auto IsLayerSupported(std::string_view const &name) -> bool {
  auto res = VK_NS::enumerateInstanceLayerProperties();

  bool found = false;
  for (auto const &ext : res.value) {
    if (std::strcmp(ext.layerName, name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

inline auto IsDeviceExtensionSupported(
    std::string_view const &name,
    VK_NS::PhysicalDevice const &device) {
  auto res = device.enumerateDeviceExtensionProperties();

  bool found = false;
  for (auto const &ext : res.value) {
    if (std::strcmp(ext.extensionName, name.data())) {
      found = true;
      break;
    }
  }

  return found;
}

}  // namespace wren::vulkan
