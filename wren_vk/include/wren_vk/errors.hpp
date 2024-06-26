#pragma once

#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren_utils/errors.hpp>

template <>
auto enum_to_string<::VK_NS::Result>(::VK_NS::Result e)
    -> std::string;

ERROR_CODE(VK_NS, Result)
ERROR_ENUM(wren, VulkanErrors, NoDevicesFound,
           QueueFamilyNotSupported)

// NOLINTNEXTLINE
#define VK_ERR_PROP(out, err)                              \
  auto [LINEIZE(res, __LINE__), out] = err;                \
  if (LINEIZE(res, __LINE__) != ::VK_NS::Result::eSuccess) \
    return tl::make_unexpected(                            \
        make_error_code(LINEIZE(res, __LINE__)));

// NOLINTNEXTLINE
#define VK_ERR_PROP_VOID(err)                              \
  VK_NS::Result LINEIZE(res, __LINE__) = err;              \
  if (LINEIZE(res, __LINE__) != ::VK_NS::Result::eSuccess) \
    return tl::make_unexpected(                            \
        make_error_code(LINEIZE(res, __LINE__)));

// NOLINTNEXTLINE
#define VK_TIE_ERR_PROP(out, err)                                   \
  ::VK_NS::Result LINEIZE(res, __LINE__) = VK_NS::Result::eSuccess; \
  std::tie(LINEIZE(res, __LINE__), out) = err;                      \
  if (LINEIZE(res, __LINE__) != ::VK_NS::Result::eSuccess)          \
    return tl::make_unexpected(                                     \
        make_error_code(LINEIZE(res, __LINE__)));
