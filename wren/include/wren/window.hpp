#pragma once

#include "SDL_video.h"
#include "event.hpp"
#include "spdlog/spdlog.h"
#include "utils/errors.hpp"
#include <SDL2/SDL.h>
#include <system_error>
#include <tl/expected.hpp>

namespace wren {

class Window {
public:
  static auto Create(const std::string &application_name)
      -> tl::expected<Window, std::error_code>;

  auto operator=(const Window &other) {
    return Window(other.window);
  }

  void Shutdown();

  void DispatchEvents(const Event::Dispatcher &dispatcher);

  auto GetRequiredVulkanExtension() const
      -> tl::expected<std::vector<std::string_view>, std::error_code>;

private:
  explicit Window(SDL_Window *window) : window(window) {}

  SDL_Window *window;
};

} // namespace wren

ERROR_ENUM(wren, WindowErrors, SDL_INIT, SDL_VULKAN_EXTENSION)
