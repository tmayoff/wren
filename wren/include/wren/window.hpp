#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <spdlog/spdlog.h>

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <wren_utils/errors.hpp>

#include "event.hpp"

namespace wren {

class Window {
 public:
  static auto Create(std::string const &application_name)
      -> expected<Window>;

  void Shutdown();

  auto CreateSurface(vk::Instance const &instance)
      -> tl::expected<vk::SurfaceKHR, std::error_code>;

  void DispatchEvents(Event::Dispatcher const &dispatcher);

  [[nodiscard]] auto GetRequiredVulkanExtension() const
      -> tl::expected<std::vector<std::string_view>, std::error_code>;

  auto GetSize() -> std::pair<int32_t, int32_t> {
    int w = 0, h = 0;
    SDL_GetWindowSize(window, &w, &h);
    return {w, h};
  }

 private:
  explicit Window(SDL_Window *window) : window(window) {}

  SDL_Window *window;
};

}  // namespace wren

ERROR_ENUM(wren, WindowErrors, SDL_INIT, SDL_VULKAN_EXTENSION)
