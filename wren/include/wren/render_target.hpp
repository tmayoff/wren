#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren/math/vector.hpp>
#include <wren/utils/enums.hpp>
#include <wren/utils/result.hpp>
#include <wren/vk/image.hpp>

#include "context.hpp"

namespace wren {

DESCRIBED_ENUM(RenderTargetType, kColour, kDepth)

class RenderTarget {
 public:
  static auto create(const std::shared_ptr<Context>& ctx)
      -> expected<std::shared_ptr<RenderTarget>>;

  static auto create(const math::Vec2f& size, ::vk::Format format,
                     ::vk::SampleCountFlagBits sample_count,
                     ::vk::ImageView image_view,
                     ::vk::ImageUsageFlags image_usage)
      -> expected<std::shared_ptr<RenderTarget>>;

  auto resize(const std::shared_ptr<Context>& ctx, const math::Vec2f& new_size)
      -> expected<void>;

  [[nodiscard]] auto view() const { return view_; }
  auto view(const ::vk::ImageView& view) { view_ = view; }

  [[nodiscard]] auto format() const { return format_; }
  auto format(const ::vk::Format& format) { format_ = format; }

  [[nodiscard]] auto size() const { return size_; }
  auto size(const math::Vec2f& size) { size_ = size; }

  [[nodiscard]] auto sample_count() const { return sample_count_; }

  [[nodiscard]] auto final_layout() const { return final_layout_; }
  auto final_layout(const ::vk::ImageLayout& layout) { final_layout_ = layout; }

  [[nodiscard]] auto usage() const { return image_usage_; }

 private:
  RenderTarget() = default;

  //! This might not be necessary, the size can stay in the render pass
  math::Vec2f size_;

  ::vk::SampleCountFlagBits sample_count_;

  ::vk::Format format_;

  std::optional<vk::Image> image_;
  ::vk::ImageView view_;

  ::vk::ImageUsageFlags image_usage_;

  ::vk::ImageLayout final_layout_ = ::vk::ImageLayout::ePresentSrcKHR;

  RenderTargetType type_ = RenderTargetType::kColour;
};

}  // namespace wren
