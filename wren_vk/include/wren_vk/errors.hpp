#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>
#include <wren_utils/errors.hpp>

namespace VK_NS {

BOOST_DESCRIBE_ENUM(
    Result, eSuccess, eNotReady, eTimeout, eEventSet, eEventReset,
    eIncomplete, eErrorOutOfHostMemory, eErrorOutOfDeviceMemory,
    eErrorInitializationFailed, eErrorDeviceLost,
    eErrorMemoryMapFailed, eErrorLayerNotPresent,
    eErrorExtensionNotPresent, eErrorFeatureNotPresent,
    eErrorIncompatibleDriver, eErrorTooManyObjects,
    eErrorFormatNotSupported, eErrorFragmentedPool, eErrorUnknown,
    eErrorOutOfPoolMemory, eErrorOutOfPoolMemoryKHR,
    eErrorInvalidExternalHandle, eErrorInvalidExternalHandleKHR,
    eErrorFragmentation, eErrorFragmentationEXT,
    eErrorInvalidOpaqueCaptureAddress, eErrorInvalidDeviceAddressEXT,
    eErrorInvalidOpaqueCaptureAddressKHR, ePipelineCompileRequired,
    eErrorPipelineCompileRequiredEXT, ePipelineCompileRequiredEXT,
    eErrorSurfaceLostKHR, eErrorNativeWindowInUseKHR, eSuboptimalKHR,
    eErrorOutOfDateKHR, eErrorIncompatibleDisplayKHR,
    eErrorValidationFailedEXT, eErrorInvalidShaderNV,
    eErrorImageUsageNotSupportedKHR,
    eErrorVideoPictureLayoutNotSupportedKHR,
    eErrorVideoProfileOperationNotSupportedKHR,
    eErrorVideoProfileFormatNotSupportedKHR,
    eErrorVideoProfileCodecNotSupportedKHR,
    eErrorVideoStdVersionNotSupportedKHR,
    eErrorInvalidDrmFormatModifierPlaneLayoutEXT,
    eErrorNotPermittedKHR, eErrorNotPermittedEXT, eThreadIdleKHR,
    eThreadDoneKHR, eOperationDeferredKHR, eOperationNotDeferredKHR,
    eErrorCompressionExhaustedEXT);

DEFINE_ERROR_IMPL("VulkanError", Result)

}  // namespace VK_NS

namespace wren {
DEFINE_ERROR("VulkanErrors", VulkanErrors, NoDevicesFound, QueueFamilyNotSupported)
}

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
