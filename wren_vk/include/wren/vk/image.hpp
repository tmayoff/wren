#pragma once

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>
#include <wren/math/vector.hpp>
#include <wren/utils/result.hpp>

namespace wren::vk {

class Image {
 public:
  static auto create(const ::vk::Device& device, const VmaAllocator& allocator,
                     const ::vk::Format& format, const math::Vec2f& size,
                     const ::vk::ImageUsageFlags& usage) -> expected<Image>;

  [[nodiscard]] auto get() const { return image_; }

 private:
  VmaAllocation alloc_{};
  ::vk::Image image_;
};

}  // namespace wren::vk
