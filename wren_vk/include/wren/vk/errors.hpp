#pragma once

#include <boost/preprocessor.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren/utils/errors.hpp>

namespace vk {

BOOST_DESCRIBE_ENUM(
    Result, eSuccess, eNotReady, eTimeout, eEventSet, eEventReset, eIncomplete,
    eErrorOutOfHostMemory, eErrorOutOfDeviceMemory, eErrorInitializationFailed,
    eErrorDeviceLost, eErrorMemoryMapFailed, eErrorLayerNotPresent,
    eErrorExtensionNotPresent, eErrorFeatureNotPresent,
    eErrorIncompatibleDriver, eErrorTooManyObjects, eErrorFormatNotSupported,
    eErrorFragmentedPool, eErrorUnknown, eErrorOutOfPoolMemory,
    eErrorOutOfPoolMemoryKHR, eErrorInvalidExternalHandle,
    eErrorInvalidExternalHandleKHR, eErrorFragmentation, eErrorFragmentationEXT,
    eErrorInvalidOpaqueCaptureAddress, eErrorInvalidDeviceAddressEXT,
    eErrorInvalidOpaqueCaptureAddressKHR, ePipelineCompileRequired,
    eErrorPipelineCompileRequiredEXT, ePipelineCompileRequiredEXT,
    eErrorSurfaceLostKHR, eErrorNativeWindowInUseKHR, eSuboptimalKHR,
    eErrorOutOfDateKHR, eErrorIncompatibleDisplayKHR, eErrorValidationFailedEXT,
    eErrorInvalidShaderNV, eErrorImageUsageNotSupportedKHR,
    eErrorVideoPictureLayoutNotSupportedKHR,
    eErrorVideoProfileOperationNotSupportedKHR,
    eErrorVideoProfileFormatNotSupportedKHR,
    eErrorVideoProfileCodecNotSupportedKHR,
    eErrorVideoStdVersionNotSupportedKHR,
    eErrorInvalidDrmFormatModifierPlaneLayoutEXT, eErrorNotPermittedKHR,
    eErrorNotPermittedEXT, eThreadIdleKHR, eThreadDoneKHR,
    eOperationDeferredKHR, eOperationNotDeferredKHR,
    eErrorCompressionExhaustedEXT);

DEFINE_ERROR_IMPL("VulkanError", Result)

}  // namespace vk

namespace wren {
DEFINE_ERROR("VulkanErrors", VulkanErrors, NoDevicesFound,
             QueueFamilyNotSupported)
}

#define VK_TRY_RESULT_1(unique, expr)               \
  auto [unique, BOOST_PP_CAT(unique, _out)] = expr; \
  if (unique != ::vk::Result::eSuccess)             \
    return std::unexpected(make_error_code(unique));

#define VK_TRY_RESULT_2(unique, out, expr) \
  auto [unique, out] = expr;               \
  if (unique != ::vk::Result::eSuccess)    \
    return std::unexpected(make_error_code(unique));

#define VK_TRY_RESULT(...)                       \
  BOOST_PP_OVERLOAD(VK_TRY_RESULT_, __VA_ARGS__) \
  (RESULT_UNIQUE_NAME(), __VA_ARGS__)

#define VK_TIE_RESULT_IMPL(unique, out, expr)   \
  ::vk::Result unique = ::vk::Result::eSuccess; \
  std::tie(unique, out) = expr;                 \
  if (unique != ::vk::Result::eSuccess)         \
    return std::unexpected(make_error_code(unique));

#define VK_TIE_RESULT(...) VK_TIE_RESULT_IMPL(RESULT_UNIQUE_NAME(), __VA_ARGS__)

#define VK_CHECK_RESULT_IMPL(unique, expr) \
  ::vk::Result unique = (expr);            \
  if ((unique) != ::vk::Result::eSuccess)  \
    return std::unexpected(make_error_code(unique));

#define VK_CHECK_RESULT(expr) VK_CHECK_RESULT_IMPL(RESULT_UNIQUE_NAME(), (expr))

// TODO Decprecate

// NOLINTNEXTLINE
#define VK_ERR_PROP(out, err)                           \
  auto [LINEIZE(res, __LINE__), out] = err;             \
  if (LINEIZE(res, __LINE__) != ::vk::Result::eSuccess) \
    return std::unexpected(make_error_code(LINEIZE(res, __LINE__)));

// NOLINTNEXTLINE
#define VK_ERR_PROP_VOID(err)                           \
  ::vk::Result LINEIZE(res, __LINE__) = err;            \
  if (LINEIZE(res, __LINE__) != ::vk::Result::eSuccess) \
    return std::unexpected(make_error_code(LINEIZE(res, __LINE__)));

// NOLINTNEXTLINE
#define VK_TIE_ERR_PROP(out, err)                               \
  ::vk::Result LINEIZE(res, __LINE__) = ::vk::Result::eSuccess; \
  std::tie(LINEIZE(res, __LINE__), out) = err;                  \
  if (LINEIZE(res, __LINE__) != ::vk::Result::eSuccess)         \
    return std::unexpected(make_error_code(LINEIZE(res, __LINE__)));
