#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_math/vector.hpp>

namespace wren {

class RenderTarget {
 public:
  math::vec2i size;

  ::vk::Format format;
  ::vk::SampleCountFlagBits sample_count;

  // ::vk::Image image;
  ::vk::ImageView image_view;

  ::vk::ImageUsageFlags image_usage;

  ::vk::ImageLayout final_layout = ::vk::ImageLayout::ePresentSrcKHR;

  auto is_colour() { return true; }
};

}  // namespace wren
