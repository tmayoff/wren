#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren/math/vector.hpp>
#include <wren/utils/enums.hpp>

namespace wren {

DESCRIBED_ENUM(RenderTargetType, kColour, kDepth)

struct RenderTarget {
  math::vec2i size;

  ::vk::Format format;
  ::vk::SampleCountFlagBits sample_count;

  vk::Image image;
  ::vk::ImageView view;

  ::vk::ImageUsageFlags image_usage;

  ::vk::ImageLayout final_layout = ::vk::ImageLayout::ePresentSrcKHR;

  RenderTargetType type = RenderTargetType::kColour;
};

}  // namespace wren
