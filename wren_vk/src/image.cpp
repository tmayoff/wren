#include "image.hpp"

namespace wren::vk {

auto Image::create(const ::vk::Device& device, const VmaAllocator& allocator,
                   const ::vk::Format& format, const math::Vec2f& size,
                   const ::vk::ImageUsageFlags& usage) -> expected<Image> {
  Image image;

  ::vk::ImageCreateInfo image_info(
      {}, ::vk::ImageType::e2D, format,
      ::vk::Extent3D(static_cast<uint32_t>(size.x()),
                     static_cast<uint32_t>(size.y()), 1),
      1, 1);
  image_info.setUsage(usage);
  image_info.setSharingMode(::vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
  auto info = static_cast<VkImageCreateInfo>(image_info);

  VkImage tmp_image = nullptr;
  vmaCreateImage(allocator, &info, &alloc_info, &tmp_image, &image.alloc_,
                 nullptr);

  image.image_ = tmp_image;

  return image;
}

}  // namespace wren::vk
