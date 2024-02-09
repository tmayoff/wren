#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

namespace wren {

class Renderer {
public:
  static auto Create() -> tl::expected<Renderer, std::error_code>;

private:
  auto CreateSwapchain() -> tl::expected<void, std::error_code>;

  vk::SwapchainKHR swapchain;
};

} // namespace wren
