#include "wren/window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>

#include <system_error>

#include "tl/expected.hpp"
#include "utils/tracy.hpp"  // IWYU pragma: export
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "wren/event.hpp"

namespace wren {

auto Window::Create(std::string const &application_name)
    -> expected<Window> {
  ZoneScoped;
  spdlog::debug("Initializing window");
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_INIT));
  }

  SDL_Window *window = SDL_CreateWindow(
      application_name.c_str(), SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, 640, 480,
      SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_INIT));
  }

  return Window(window);
}

void Window::Shutdown() { SDL_Quit(); }

auto Window::CreateSurface(VK_NS::Instance const &instance)
    -> tl::expected<VK_NS::SurfaceKHR, std::error_code> {
  VkSurfaceKHR surface{};
  SDL_Vulkan_CreateSurface(window, instance, &surface);

  return surface;
}

auto Window::GetRequiredVulkanExtension() const
    -> tl::expected<std::vector<std::string_view>, std::error_code> {
  uint32_t count = 0;
  bool res =
      SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  std::vector<char const *> extensions(count);
  res = SDL_Vulkan_GetInstanceExtensions(window, &count,
                                         extensions.data());
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  return std::vector<std::string_view>{extensions.begin(),
                                       extensions.end()};
}

void Window::DispatchEvents(Event::Dispatcher const &dispatcher) {
  ZoneScoped;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        dispatcher.dispatch(Event::WindowClose{});
        break;

        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              dispatcher.dispatch(Event::WindowResized{
                  {static_cast<float>(event.window.data1),
                   static_cast<float>(event.window.data2)}});
              break;
          }
          break;
        case SDL_MOUSEMOTION:
          dispatcher.dispatch(
              Event::MouseMoved{static_cast<float>(event.motion.x),
                                static_cast<float>(event.motion.y)});
          break;
      }
    }
  }
}

}  // namespace wren
