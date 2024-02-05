#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class GraphicsContext {
public:
  GraphicsContext();

private:
  auto CreateInstance() -> tl::expected<void, std::error_code>;

  vk::Instance instance;
};

} // namespace wren

