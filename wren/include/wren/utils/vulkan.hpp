#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>

VKAPI_ATTR auto VKAPI_CALL vkCreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pMessenger) -> VkResult;

namespace wren::vulkan {

VKAPI_ATTR auto VKAPI_CALL
DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) -> VkBool32;

inline auto IsExtensionSupport(const std::string_view &name) -> bool {
  auto res = vk::enumerateInstanceExtensionProperties();

  bool found = false;
  for (const auto &ext : res.value) {
    if (ext.extensionName == name) {
      found = true;
      break;
    }
  }

  return found;
}

inline auto IsLayerSupported(const std::string_view &name) -> bool {
  auto res = vk::enumerateInstanceLayerProperties();

  bool found = false;
  for (const auto &ext : res.value) {
    if (ext.layerName == name) {
      found = true;
      break;
    }
  }

  return found;
}

} // namespace wren::vulkan
