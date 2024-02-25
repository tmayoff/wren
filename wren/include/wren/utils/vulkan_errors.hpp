#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

#include "enums.hpp"
#include "errors.hpp"

template <>
auto enum_to_string<vk::Result>(vk::Result e) -> std::string;

ERROR_CODE(vk, Result)
ERROR_ENUM(wren, VulkanErrors, NoDevicesFound,
           QueueFamilyNotSupported)
