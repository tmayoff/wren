#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class RenderTarget {
 public:
  vk::Extent2D size;
  vk::Format format;
  vk::SampleCountFlagBits sample_count;

  // vk::Image image;
  vk::ImageView image_view;

  auto is_colour() { return true; }
};

}  // namespace wren
