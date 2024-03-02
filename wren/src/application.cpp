#include "wren/application.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#if __has_include(<Tracy/tracy/Tracy.hpp>)
  #include <Tracy/tracy/Tracy.hpp>
#else
  #include <tracy/Tracy.hpp>
#endif
#include <memory>
#include <tl/expected.hpp>

#include "wren/context.hpp"
#include "wren/event.hpp"
#include "wren/graphics_context.hpp"
#include "wren/renderer.hpp"

namespace wren {

auto Application::Create(const std::string &application_name)
    -> tl::expected<std::shared_ptr<Application>, std::error_code> {
  spdlog::set_level(spdlog::level::debug);
  ZoneScoped;

  spdlog::debug("Initializing application");

  auto window = Window::Create(application_name);
  if (!window.has_value()) {
    return tl::make_unexpected(window.error());
  }

  auto extensions = window->GetRequiredVulkanExtension();
  if (!extensions.has_value())
    return tl::make_unexpected(extensions.error());

  auto graphics_context_res =
      GraphicsContext::Create(application_name, *extensions);
  if (!graphics_context_res.has_value())
    return tl::make_unexpected(graphics_context_res.error());
  auto graphics_context = graphics_context_res.value();

  spdlog::trace("Created graphics context");

  {
    auto res = window->CreateSurface(graphics_context->Instance());
    if (!res.has_value()) return tl::make_unexpected(res.error());
    graphics_context->Surface(res.value());
  }

  {
    auto res = graphics_context->SetupDevice();
    if (!res.has_value()) return tl::make_unexpected(res.error());
  }

  auto ctx = std::make_shared<Context>(*window, Event::Dispatcher(),
                                       graphics_context);

  auto renderer = Renderer::Create(ctx);
  if (!renderer.has_value())
    return tl::make_unexpected(renderer.error());

  return std::shared_ptr<Application>(
      new Application(ctx, renderer.value()));
}

Application::Application(const std::shared_ptr<Context> &ctx,
                         const std::shared_ptr<Renderer> &renderer)
    : ctx(ctx), renderer(renderer), running(true) {}

void Application::run() {
  this->ctx->event_dispatcher.on<Event::WindowClose>([this](auto &w) {
    spdlog::debug("{}", w.debug_name);
    this->running = false;
  });

  this->ctx->event_dispatcher.on<Event::WindowResized>(
      [](const Event::WindowResized &size) {
        spdlog::debug("{} ({}, {})", size.debug_name, size.width,
                      size.height);
      });

  while (running) {
    FrameMark;
    ctx->window.DispatchEvents(ctx->event_dispatcher);

    ctx->renderer->draw();
  }
}

}  // namespace wren
