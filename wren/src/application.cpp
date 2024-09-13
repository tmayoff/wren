#include "wren/application.hpp"

#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <tl/expected.hpp>
#include <wren_utils/errors.hpp>

#include "wren/context.hpp"
#include "wren/event.hpp"
#include "wren/graphics_context.hpp"
#include "wren/renderer.hpp"
#include "wren/utils/tracy.hpp"  // IWYU pragma: export

namespace wren {

auto Application::Create(std::string const &application_name)
    -> expected<std::shared_ptr<Application>> {
  spdlog::set_level(spdlog::level::debug);
  ZoneScoped;

  spdlog::debug("Initializing application");

  auto window = Window::Create(application_name);
  if (!window.has_value()) {
    return tl::make_unexpected(window.error());
  }

  TRY_RESULT(auto extensions, window->GetRequiredVulkanExtension());

  TRY_RESULT(auto graphics_context,
           GraphicsContext::Create(application_name, extensions));
  spdlog::trace("Created graphics context");

  TRY_RESULT(const auto surface,
           window->CreateSurface(graphics_context->Instance()));
  graphics_context->Surface(surface);

  TRY_RESULT(graphics_context->SetupDevice());

  auto ctx = std::make_shared<Context>(*window, Event::Dispatcher(),
                                       graphics_context);

  TRY_RESULT(auto renderer, Renderer::New(ctx));

  return std::shared_ptr<Application>(new Application(ctx, renderer));
}

void Application::add_callback_to_phase(CallbackPhase phase,
                                        phase_cb_t const &cb) {
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

Application::Application(std::shared_ptr<Context> const &ctx,
                         std::shared_ptr<Renderer> const &renderer)
    : ctx(ctx), renderer(renderer), running(true) {}

void Application::run() {
  this->ctx->event_dispatcher.on<Event::WindowClose>([this](auto &w) {
    spdlog::debug("{}", w.debug_name);
    this->running = false;
  });

  this->ctx->event_dispatcher.on<Event::WindowResized>(
      [](Event::WindowResized const &size) {
        spdlog::debug("{} ({}, {})", size.debug_name, size.width,
                      size.height);
      });

  for (auto const &cb : startup_phase) {
    if (cb) cb();
  }

  while (running) {
    FrameMark;
    ctx->window.DispatchEvents(ctx->event_dispatcher);

    for (auto const &cb : update_phase) {
      if (cb) cb();
    }

    ctx->renderer->draw();
  }

  for (auto const &cb : update_phase) {
    if (cb) cb();
  }
}

}  // namespace wren
