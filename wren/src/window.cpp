#include "wren/window.hpp"

#include "SDL_events.h"
#include "SDL_video.h"
#include "tl/expected.hpp"
#include "wren/event.hpp"
#include <SDL2/SDL.h>
#include <functional>

namespace wren {

auto Window::Create() -> tl::expected<Window, std::error_code> {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_INIT_FAILED));
  }

  SDL_Window *window = SDL_CreateWindow("TITLE", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  if (window == nullptr) {
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_INIT_FAILED));
  }

  return Window(window);
}

void Window::DispatchEvents(const Event::Dispatcher &dispatcher) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT: {
      dispatcher.dispatch(Event::WindowClose{});
      break;
    }
    }
  }
}

} // namespace wren
