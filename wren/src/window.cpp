#include "wren/window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL_error.h>
#include <SDL_mouse.h>

#include <tracy/Tracy.hpp>
#include <tl/expected.hpp>

#include "keycode.hpp"
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
    spdlog::error("Failed to create window: {}", SDL_GetError());
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_WINDOW));
  }

  return Window(window);
}

void Window::Shutdown() { SDL_Quit(); }

auto Window::CreateSurface(::vk::Instance const &instance)
    -> expected<::vk::SurfaceKHR> {
  VkSurfaceKHR surface{};
  SDL_Vulkan_CreateSurface(window, instance, &surface);

  return surface;
}

auto Window::GetRequiredVulkanExtension() const
    -> expected<std::vector<std::string_view>> {
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

auto SDLMouse_WrenMouse(uint8_t sdl_button) {
  MouseCode code = MouseCode::Left;
  if (sdl_button == SDL_BUTTON_LEFT) return MouseCode::Left;
  if (sdl_button == SDL_BUTTON_MIDDLE)
    return code = MouseCode::Middle;
  if (sdl_button == SDL_BUTTON_RIGHT) return code = MouseCode::Right;

  return MouseCode::Middle;
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

          dispatcher.dispatch(Event::MouseMoved{
              static_cast<float>(event.motion.xrel),
              static_cast<float>(event.motion.yrel), true});
          break;

        case SDL_MOUSEBUTTONDOWN: {
          dispatcher.dispatch(Event::MouseButtonDown{
              SDLMouse_WrenMouse(event.button.button)});
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          dispatcher.dispatch(Event::MouseButtonUp{
              SDLMouse_WrenMouse(event.button.button)});
          break;
        }
      }
    }
  }
}

}  // namespace wren
