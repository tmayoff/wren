#pragma once

#include "event.hpp"
#include "graphics_context.hpp"
#include "window.hpp"
#include <memory>

namespace wren {

class Renderer;

struct Context {
  Window window;
  Event::Dispatcher event_dispatcher;
  GraphicsContext graphics_context;
  std::shared_ptr<Renderer> renderer;
};

} // namespace wren
