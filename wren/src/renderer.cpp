#include "wren/renderer.hpp"
#include <system_error>
#include <tl/expected.hpp>

namespace wren {

auto Renderer::Create() -> tl::expected<Renderer, std::error_code> {
  Renderer renderer;

  auto res = renderer.CreateSwapchain();
  if (!res.has_value())
    return tl::make_unexpected(res.error());

  return renderer;
}

auto Renderer::CreateSwapchain() -> tl::expected<void, std::error_code> {}

} // namespace wren
