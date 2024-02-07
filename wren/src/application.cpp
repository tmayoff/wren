#include "wren/application.hpp"
#include "wren/context.hpp"
#include "wren/event.hpp"
#include "wren/graphics_context.hpp"
#include <tl/expected.hpp>

#include <memory>
#include <spdlog/spdlog.h>

namespace wren {

auto Application::Create(const std::string &application_name)
    -> tl::expected<std::shared_ptr<Application>, std::error_code> {
  spdlog::set_level(spdlog::level::debug);

  spdlog::debug("Initializing application");

  const auto &window = Window::Create(application_name);
  if (!window.has_value()) {
    return tl::make_unexpected(window.error());
  }

  auto extensions = window->GetRequiredVulkanExtension();
  if (!extensions.has_value())
    return tl::make_unexpected(extensions.error());

  const auto graphics_context =
      GraphicsContext::Create(application_name, *extensions);
  if (!graphics_context.has_value())
    return tl::make_unexpected(graphics_context.error());

  auto ctx = std::make_shared<Context>(*window, Event::Dispatcher(),
                                       *graphics_context);

  return std::shared_ptr<Application>(new Application(ctx));
}

Application::Application(const std::shared_ptr<Context> &ctx)
    : ctx(ctx), running(true) {}

void Application::run() {
  this->ctx->event_dispatcher.on<Event::WindowClose>([this]() {
    spdlog::info("Window closed");
    this->running = false;
  });

  while (running) {
    ctx->window.DispatchEvents(ctx->event_dispatcher);
  }
}

} // namespace wren
