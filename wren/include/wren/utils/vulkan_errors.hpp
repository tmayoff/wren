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

// NOLINTNEXTLINE
#define VK_ERR_PROP(out, err)                         \
  auto [LINEIZE(res, __LINE__), out] = err;           \
  if (LINEIZE(res, __LINE__) != vk::Result::eSuccess) \
    return tl::make_unexpected(                       \
        make_error_code(LINEIZE(res, __LINE__)));

// NOLINTNEXTLINE
#define VK_TIE_ERR_PROP(out, err)                           \
  vk::Result LINEIZE(res, __LINE__) = vk::Result::eSuccess; \
  std::tie(LINEIZE(res, __LINE__), out) = err;              \
  if (LINEIZE(res, __LINE__) != vk::Result::eSuccess)       \
    return tl::make_unexpected(                             \
        make_error_code(LINEIZE(res, __LINE__)));
