#pragma once

#include "errors.hpp"
#include "enums.hpp"
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

template <> auto enum_to_string<vk::Result>(vk::Result e) -> std::string;

ERROR_CODE(vk, Result)
ERROR_ENUM(wren, VulkanErrors, NoDevicesFound, QueueFamilyNotSupported)
