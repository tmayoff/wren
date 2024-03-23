#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class RenderTarget {
 public:
  VK_NS::Extent2D size;
  VK_NS::Format format;
  VK_NS::SampleCountFlagBits sample_count;

  // VK_NS::Image image;
  VK_NS::ImageView image_view;

  auto is_colour() { return true; }
};

}  // namespace wren
