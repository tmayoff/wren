#pragma once

#include "errors.hpp"
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

template <> auto enum_to_string<vk::Result>(vk::Result e) -> std::string {
  return vk::to_string(e);
}

ERROR_CODE(vk, Result, eInitializationFailed, eSuccess, eOutofHostMemory)
ERROR_ENUM(wren, VulkanErrors, NoDevicesFound, QueueFamilyNotSupported)
