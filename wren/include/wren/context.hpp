#pragma once

#include <memory>

#include "event.hpp"
#include "graphics_context.hpp"
#include "window.hpp"

namespace wren {

class Renderer;

struct Context {
  Window window;
  Event::Dispatcher event_dispatcher;
  std::shared_ptr<GraphicsContext> graphics_context;
  std::shared_ptr<Renderer> renderer;
};

}  // namespace wren
