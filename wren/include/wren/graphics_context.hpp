#pragma once

#include <vk_mem_alloc.h>

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "wren/utils/device.hpp"
#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan.hpp"

namespace wren {

class GraphicsContext {
 public:
  static auto Create(
      std::string const &application_name,
      std::vector<std::string_view> const &requested_extensions = {},
      std::vector<std::string_view> const &requested_layers = {})
      -> tl::expected<std::shared_ptr<GraphicsContext>,
                      std::error_code>;

  GraphicsContext(GraphicsContext const &) = delete;
  GraphicsContext(GraphicsContext &&) = delete;
  auto operator=(GraphicsContext const &) = delete;
  auto operator=(GraphicsContext &&) = delete;
  ~GraphicsContext();

  auto InitializeSurface() -> tl::expected<void, std::error_code>;

  [[nodiscard]] auto Instance() const { return instance; }
  void Surface(VK_NS::SurfaceKHR const &surface) {
    this->surface = surface;
  }
  [[nodiscard]] auto Surface() const -> VK_NS::SurfaceKHR {
    return surface;
  }

  [[nodiscard]] auto PhysicalDevice() const -> VK_NS::PhysicalDevice {
    return physical_device;
  }

  [[nodiscard]] auto Device() const -> vulkan::Device const & {
    return device;
  }

  [[nodiscard]] auto allocator() const { return allocator_; }

  auto SetupDevice() -> tl::expected<void, std::error_code>;

  auto GetSwapchainSupport() {
    return vulkan::GetSwapchainSupportDetails(physical_device,
                                              surface);
  }

  auto FindQueueFamilyIndices() {
    return vulkan::Queue::FindQueueFamilyIndices(physical_device,
                                                 surface);
  }

 private:
  GraphicsContext() = default;

  auto CreateInstance(
      std::string const &application_name,
      std::vector<std::string_view> const &requested_extensions = {},
      std::vector<std::string_view> const &requested_layers = {})
      -> tl::expected<void, std::error_code>;

  auto CreateAllocator() -> tl::expected<void, std::error_code>;

  auto CreateDevice() -> tl::expected<void, std::error_code>;
  auto PickPhysicalDevice() -> tl::expected<void, std::error_code>;
  auto IsDeviceSuitable(VK_NS::PhysicalDevice const &device) -> bool;

  VK_NS::Instance instance;
  VK_NS::PhysicalDevice physical_device;

  vulkan::Device device;

  VK_NS::SurfaceKHR surface;

  VmaAllocator allocator_{};

#ifdef WREN_DEBUG
  auto CreateDebugMessenger() -> tl::expected<void, std::error_code>;
  VK_NS::DebugUtilsMessengerEXT debug_messenger;
#endif
};

}  // namespace wren
