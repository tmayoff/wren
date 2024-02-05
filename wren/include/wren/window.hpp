#pragma once

#include "SDL_video.h"
#include "event.hpp"
#include "utils/errors.hpp"
#include <SDL2/SDL.h>
#include <tl/expected.hpp>

ERROR_ENUM(wren, WindowErrors, SDL_INIT_FAILED)

namespace wren {

class Window {
public:
  static auto Create() -> tl::expected<Window, std::error_code>;

  void DispatchEvents(const Event::Dispatcher &dispatcher);

private:
  explicit Window(SDL_Window *window) : window(window) {}

  SDL_Window *window;
};

} // namespace wren
