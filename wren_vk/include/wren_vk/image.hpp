#pragma once

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>
#include <wren/utils/errors.hpp>
#include <wren_math/vector.hpp>

namespace wren::vk {

class Image {
 public:
  static auto create(const ::vk::Device& device, const VmaAllocator& allocator,
                     const ::vk::Format& format, const math::vec2i& size,
                     const ::vk::ImageUsageFlags& usage) -> expected<Image>;

  [[nodiscard]] auto get() const { return image_; }

 private:
  VmaAllocation alloc_{};
  ::vk::Image image_;
};

}  // namespace wren::vk
