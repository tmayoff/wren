#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren::vulkan {

inline bool IsExtensionSupport(const std::string_view &name) {
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

inline bool IsLayerSupported(const std::string_view &name) {
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
