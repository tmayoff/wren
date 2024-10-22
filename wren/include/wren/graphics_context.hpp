#pragma once

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "wren/utils/device.hpp"
#include "wren/utils/queue.hpp"
#include "wren/utils/vulkan.hpp"

namespace wren {

class GraphicsContext {
 public:
  static auto Create(
      const std::string &application_name,
      const std::vector<std::string_view> &requested_extensions = {},
      const std::vector<std::string_view> &requested_layers = {})
      -> expected<std::shared_ptr<GraphicsContext>>;

  GraphicsContext(const GraphicsContext &) = delete;
  GraphicsContext(GraphicsContext &&) = delete;
  auto operator=(const GraphicsContext &) = delete;
  auto operator=(GraphicsContext &&) = delete;
  ~GraphicsContext();

  auto InitializeSurface() -> expected<void>;

  [[nodiscard]] auto Instance() const { return instance; }
  void Surface(const ::vk::SurfaceKHR &surface) { this->surface = surface; }
  [[nodiscard]] auto Surface() const -> ::vk::SurfaceKHR { return surface; }

  [[nodiscard]] auto PhysicalDevice() const -> ::vk::PhysicalDevice {
    return physical_device;
  }

  [[nodiscard]] auto Device() const -> const vulkan::Device & { return device; }

  [[nodiscard]] auto allocator() const { return allocator_; }

  auto SetupDevice() -> expected<void>;

  auto GetSwapchainSupport() {
    return vulkan::get_swapchain_support_details(physical_device, surface);
  }

  auto FindQueueFamilyIndices() {
    return vulkan::Queue::find_queue_family_indices(physical_device, surface);
  }

 private:
  GraphicsContext() = default;

  auto CreateInstance(
      const std::string &application_name,
      const std::vector<std::string_view> &requested_extensions = {},
      const std::vector<std::string_view> &requested_layers = {})
      -> expected<void>;

  auto CreateAllocator() -> expected<void>;

  auto CreateDevice() -> expected<void>;
  auto PickPhysicalDevice() -> expected<void>;
  auto IsDeviceSuitable(const ::vk::PhysicalDevice &device) -> bool;

  ::vk::Instance instance;
  ::vk::PhysicalDevice physical_device;

  vulkan::Device device;

  ::vk::SurfaceKHR surface;

  VmaAllocator allocator_{};

#ifdef WREN_DEBUG
  auto CreateDebugMessenger() -> expected<void>;
  ::vk::DebugUtilsMessengerEXT debug_messenger;
#endif
};

}  // namespace wren
