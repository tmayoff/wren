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
  vk::SurfaceCapabilitiesKHR surface_capabilites;
  std::vector<vk::SurfaceFormatKHR> surface_formats;
  std::vector<vk::PresentModeKHR> present_modes;
};

auto LoadFunctions(vk::Instance const &instance)
    -> tl::expected<void, std::error_code>;

auto GetSwapchainSupportDetails(
    vk::PhysicalDevice const &physical_device,
    vk::SurfaceKHR const &surface)
    -> tl::expected<SwapchainSupportDetails, std::error_code>;

VKAPI_ATTR auto VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData,
    void *pUserData) -> VkBool32;

inline auto IsExtensionSupport(std::string_view const &name) -> bool {
  auto res = vk::enumerateInstanceExtensionProperties();

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
  auto res = vk::enumerateInstanceLayerProperties();

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
    std::string_view const &name, vk::PhysicalDevice const &device) {
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
