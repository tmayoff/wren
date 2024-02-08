#pragma once

#include "wren/utils/device.hpp"
#include "wren/utils/errors.hpp"
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class GraphicsContext {
public:
  static auto
  Create(const std::string &application_name,
         const std::vector<std::string_view> &requested_extensions = {},
         const std::vector<std::string_view> &requested_layers = {})
      -> tl::expected<GraphicsContext, std::error_code>;

  auto InitializeSurface() -> tl::expected<void, std::error_code>;

  void Shutdown();

  [[nodiscard]] auto Instance() const { return instance; }
  void Surface(const vk::SurfaceKHR &surface) { this->surface = surface; }

  auto CreateDevice() -> tl::expected<void, std::error_code>;

private:
  auto
  CreateInstance(const std::string &application_name,
                 const std::vector<std::string_view> &requested_extensions = {},
                 const std::vector<std::string_view> &requested_layers = {})
      -> tl::expected<void, std::error_code>;

  auto PickPhysicalDevice() -> tl::expected<void, std::error_code>;
  auto IsDeviceSuitable(const vk::PhysicalDevice &device) -> bool;

  vk::Instance instance;
  vk::PhysicalDevice physical_device;

  vulkan::Device device;

  vk::SurfaceKHR surface;

#ifdef WREN_DEBUG
  auto CreateDebugMessenger() -> tl::expected<void, std::error_code>;
  vk::DebugUtilsMessengerEXT debug_messenger;
#endif
};

} // namespace wren
