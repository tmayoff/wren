#pragma once

#include "event.hpp"
#include "graphics_context.hpp"
#include "renderer.hpp"
#include "window.hpp"
#include <memory>

namespace wren {

struct Context {
  Window window;
  Event::Dispatcher event_dispatcher;
  GraphicsContext graphics_context;
  Renderer renderer;
};

} // namespace wren
