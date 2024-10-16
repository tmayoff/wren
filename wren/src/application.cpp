#include "wren/application.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <tl/expected.hpp>
#include <wren/utils/errors.hpp>

#include "wren/context.hpp"
#include "wren/event.hpp"
#include "wren/graphics_context.hpp"
#include "wren/renderer.hpp"
#include "wren/utils/tracy.hpp"  // IWYU pragma: export

namespace wren {

auto Application::Create(const std::string &application_name)
    -> expected<std::shared_ptr<Application>> {
  ZoneScoped;

  spdlog::debug("Initializing application");

  auto window = Window::create(application_name);
  if (!window.has_value()) {
    return tl::make_unexpected(window.error());
  }

  TRY_RESULT(auto extensions, window->get_required_vulkan_extension());

  TRY_RESULT(auto graphics_context,
             GraphicsContext::Create(application_name, extensions));
  spdlog::trace("Created graphics context");

  TRY_RESULT(const auto surface,
             window->create_surface(graphics_context->Instance()));
  graphics_context->Surface(surface);

  TRY_RESULT(graphics_context->SetupDevice());

  auto ctx =
      std::make_shared<Context>(*window, event::Dispatcher(), graphics_context);

  TRY_RESULT(auto renderer, Renderer::New(ctx));

  return std::shared_ptr<Application>(new Application(ctx, renderer));
}

void Application::add_callback_to_phase(CallbackPhase phase,
                                        const phase_cb_t &cb) {
  switch (phase) {
    case CallbackPhase::Startup:
      startup_phase.emplace_back(cb);
      break;
    case CallbackPhase::Update:
      update_phase.emplace_back(cb);
      break;
    case CallbackPhase::Shutdown:
      shutdown_phase.emplace_back(cb);
      break;
  }
}

Application::Application(const std::shared_ptr<Context> &ctx,
                         const std::shared_ptr<Renderer> &renderer)
    : ctx(ctx), renderer(renderer), running(true) {}

void Application::run() {
  this->ctx->event_dispatcher.on<event::WindowClose>([this](auto &w) {
    spdlog::debug("{}", w.debug_name);
    this->running = false;
  });

  this->ctx->event_dispatcher.on<event::WindowResized>(
      [](const event::WindowResized &size) {
        spdlog::debug("{} ({}, {})", size.debug_name, size.width, size.height);
      });

  for (const auto &cb : startup_phase) {
    if (cb) cb();
  }

  while (running) {
    FrameMark;
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
    ctx->window.dispatch_events(ctx->event_dispatcher);

    for (const auto &cb : update_phase) {
      if (cb) cb();
    }

    ctx->renderer->draw();
  }

  for (const auto &cb : update_phase) {
    if (cb) cb();
  }
}

}  // namespace wren
