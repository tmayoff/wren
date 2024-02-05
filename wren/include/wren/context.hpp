#pragma once

#include "event.hpp"
#include "window.hpp"
#include <memory>

namespace wren {

struct Context {
  Window window;
  Event::Dispatcher event_dispatcher;
};

} // namespace wren
