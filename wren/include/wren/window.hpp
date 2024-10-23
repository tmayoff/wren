#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <spdlog/spdlog.h>

#include <boost/describe/enum.hpp>
#include <vulkan/vulkan.hpp>
#include <wren/utils/result.hpp>

#include "event.hpp"

namespace wren {

DEFINE_ERROR("WindowErrors", WindowErrors, SDL_INIT, SDL_WINDOW,
             SDL_VULKAN_EXTENSION);

class Window {
 public:
  static auto create(const std::string &application_name) -> expected<Window>;

  void shutdown();

  auto create_surface(const ::vk::Instance &instance)
      -> expected<::vk::SurfaceKHR>;

  void dispatch_events(const event::Dispatcher &dispatcher);

  [[nodiscard]] auto get_required_vulkan_extension() const
      -> expected<std::vector<std::string_view>>;

  auto get_size() -> std::pair<int32_t, int32_t> {
    int w = 0, h = 0;
    SDL_GetWindowSize(window_, &w, &h);
    return {w, h};
  }

  [[nodiscard]] auto native_handle() const { return window_; }

 private:
  explicit Window(SDL_Window *window) : window_(window) {}

  SDL_Window *window_;
};

}  // namespace wren
