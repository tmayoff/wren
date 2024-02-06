#include "wren/window.hpp"

#include "SDL_events.h"
#include "SDL_video.h"
#include "tl/expected.hpp"
#include "wren/event.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <functional>
#include <system_error>

namespace wren {

auto Window::Create(const std::string &application_name)
    -> tl::expected<Window, std::error_code> {
  spdlog::debug("Initializing window");
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_INIT));
  }

  SDL_Window *window =
      SDL_CreateWindow(application_name.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_VULKAN);
  if (window == nullptr) {
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_INIT));
  }

  return Window(window);
}

void Window::Shutdown() { SDL_Quit(); }

auto Window::GetRequiredVulkanExtension() const
    -> tl::expected<std::vector<std::string_view>, std::error_code> {
  uint32_t count = 0;
  bool res = SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  std::vector<const char *> extensions(count);
  res = SDL_Vulkan_GetInstanceExtensions(window, &count, extensions.data());
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  return std::vector<std::string_view>{extensions.begin(), extensions.end()};
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
