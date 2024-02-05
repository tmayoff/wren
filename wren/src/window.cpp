#include "wren/window.hpp"

#include "SDL_video.h"
#include "tl/expected.hpp"
#include <SDL2/SDL.h>

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

} // namespace wren
