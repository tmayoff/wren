#pragma once

#include "errors.hpp"
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

template <> std::string enum_to_string<vk::Result>(vk::Result e) {
  return vk::to_string(e);
}

ERROR_CODE(vk, Result, eInitializationFailed, eSuccess, eOutofHostMemory)
