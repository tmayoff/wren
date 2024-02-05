#include "wren/application.hpp"
#include "tl/expected.hpp"

#include <spdlog/spdlog.h>

namespace wren {

auto Application::Create() -> tl::expected<Application, std::error_code> {
  const auto window = Window::Create();
  if (!window.has_value()) {
    return tl::make_unexpected( window.error());
  }
}

Application::Application() { spdlog::info("Initializing application"); }

void Application::run() {}

} // namespace wren
