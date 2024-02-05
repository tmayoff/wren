#include "wren/application.hpp"
#include "tl/expected.hpp"
#include "wren/context.hpp"
#include "wren/event.hpp"

#include <memory>
#include <spdlog/spdlog.h>

namespace wren {

auto Application::Create() -> tl::expected<Application, std::error_code> {
  const auto window = Window::Create();
  if (!window.has_value()) {
    return tl::make_unexpected(window.error());
  }

  auto ctx = std::make_shared<Context>(window.value());

  return Application(ctx);
}

Application::Application(const std::shared_ptr<Context> &ctx)
    : ctx(ctx), running(true) {
  spdlog::info("Initializing application");
}

void Application::run() {
  this->ctx->event_dispatcher.on<Event::WindowClose>([this]() {
    spdlog::info("Window closed");
    this->running = false;
  });

  while (running) {
    ctx->window.DispatchEvents(ctx->event_dispatcher);
  }

  spdlog::info("Application closing");
}

} // namespace wren
