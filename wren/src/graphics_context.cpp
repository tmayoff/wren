#include "wren/graphics_context.hpp"
#include "spdlog/spdlog.h"
#include "wren/utils/vulkan.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include <algorithm>
#include <spdlog/fmt/ranges.h>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace wren {

auto GraphicsContext::Create(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers)
    -> tl::expected<GraphicsContext, std::error_code> {
  GraphicsContext graphics_context;

  auto res = graphics_context.CreateInstance(
      application_name, requested_extensions, requested_layers);
  if (!res.has_value()) {
    return tl::make_unexpected(res.error());
  }

  return graphics_context;
}

auto GraphicsContext::CreateInstance(
    const std::string &application_name,
    const std::vector<std::string_view> &requested_extensions,
    const std::vector<std::string_view> &requested_layers)
    -> tl::expected<void, std::error_code> {
  const auto appInfo = vk::ApplicationInfo(application_name.c_str(), 1, "wren",
                                           1, VK_API_VERSION_1_1);

  spdlog::debug("Requesting extensions");
  std::vector<const char *> extensions;
  for (const auto &ext : requested_extensions) {
    spdlog::debug("\t{}", ext);
    if (vulkan::IsExtensionSupport(ext)) {
      extensions.push_back(ext.data());
    } else {
      spdlog::warn("Requested vulkan extension {}, is not supported", ext);
    }
  }

  std::vector<const char *> layers;
  for (const auto &ext : requested_layers) {
    if (vulkan::IsLayerSupported(ext)) {
      extensions.push_back(ext.data());
    } else {
      spdlog::warn("Requested vulkan layer {}, is not supported", ext);
    }
  }

  const vk::InstanceCreateInfo createInfo({}, &appInfo, layers, extensions);

  vk::ResultValue<vk::Instance> res = vk::createInstance(createInfo);
  if (res.result != vk::Result::eSuccess) {
    return tl::make_unexpected(make_error_code(res.result));
  }

  instance = res.value;

  return {};
}

} // namespace wren
