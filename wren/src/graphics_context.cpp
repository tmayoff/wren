#include "wren/graphics_context.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace wren {

GraphicsContext::GraphicsContext() { CreateInstance(); }

auto GraphicsContext::CreateInstance() -> tl::expected<void, std::error_code> {
  const auto appInfo =
      vk::ApplicationInfo("APP_NAME", 1, "wren", 1, VK_API_VERSION_1_1);

  const vk::InstanceCreateInfo createInfo({}, &appInfo);

  vk::ResultValue<vk::Instance> res = vk::createInstance(createInfo);
  if (res.result != vk::Result::eSuccess) {
    return tl::unexpected(make_error_code(res.result));
  }

  instance = res.value;

  return {};
}

} // namespace wren
