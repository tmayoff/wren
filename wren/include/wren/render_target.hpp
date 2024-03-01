#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class RenderTarget {
 public:
  vk::Extent2D size;
  vk::Format format;
  vk::SampleCountFlagBits sample_count;

  std::vector<vk::ImageView> image_views;

  auto is_colour() { return true; }
};

}  // namespace wren
