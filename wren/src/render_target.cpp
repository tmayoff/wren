#include "render_target.hpp"

#include "renderer.hpp"

namespace wren {

auto RenderTarget::create(const std::shared_ptr<Context> &ctx)
    -> expected<std::shared_ptr<RenderTarget>> {
  auto target =
      std::shared_ptr<wren::RenderTarget>(new RenderTarget());  // NOLINT

  target->size_ = {512, 512};
  target->sample_count_ = ::vk::SampleCountFlagBits::e1;
  target->format_ = ::vk::Format::eB8G8R8A8Srgb;
  target->image_usage_ = ::vk::ImageUsageFlagBits::eColorAttachment |
                         ::vk::ImageUsageFlagBits::eSampled;

  TRY_RESULT(
      target->image_,
      vk::Image::create(ctx->graphics_context->Device().get(),
                        ctx->graphics_context->allocator(), target->format_,
                        target->size_, target->image_usage_));

  // transition image
  TRY_RESULT(ctx->renderer->submit_command_buffer(
      [&image = target->image_](const ::vk::CommandBuffer &cmd_buf) {
        ::vk::ImageMemoryBarrier barrier(
            ::vk::AccessFlagBits::eTransferRead,
            ::vk::AccessFlagBits::eMemoryRead, ::vk::ImageLayout::eUndefined,
            ::vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED, image.get(),
            ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1,
                                        0, 1));

        cmd_buf.pipelineBarrier(::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::DependencyFlags(), {}, {}, barrier);
      }));

  ::vk::ImageViewCreateInfo image_view_info(
      {}, target->image_.get(), ::vk::ImageViewType::e2D, target->format_, {},
      ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                  1));
  VK_TIE_RESULT(
      target->view_,
      ctx->graphics_context->Device().get().createImageView(image_view_info));

  return target;
}

auto RenderTarget::create(const math::Vec2f &size, ::vk::Format format,
                          ::vk::SampleCountFlagBits sample_count,
                          ::vk::ImageView image_view,
                          ::vk::ImageUsageFlags image_usage)
    -> expected<std::shared_ptr<RenderTarget>> {
  auto target = std::shared_ptr<wren::RenderTarget>(new RenderTarget());

  target->size_ = size;
  target->sample_count_ = sample_count;
  target->format_ = format;
  target->image_usage_ = image_usage;

  return target;
}

auto RenderTarget::resize(const std::shared_ptr<Context> &ctx,
                          const math::Vec2f &new_size) -> expected<void> {
  size_ = new_size;

  // Delete image

  // Create a new image

  TRY_RESULT(image_, vk::Image::create(ctx->graphics_context->Device().get(),
                                       ctx->graphics_context->allocator(),
                                       format_, size_, image_usage_));

  // transition image
  TRY_RESULT(ctx->renderer->submit_command_buffer(
      [this](const ::vk::CommandBuffer &cmd_buf) {
        ::vk::ImageMemoryBarrier barrier(
            ::vk::AccessFlagBits::eTransferRead,
            ::vk::AccessFlagBits::eMemoryRead, ::vk::ImageLayout::eUndefined,
            ::vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED, image_.get(),
            ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1,
                                        0, 1));

        cmd_buf.pipelineBarrier(::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::DependencyFlags(), {}, {}, barrier);
      }));

  ::vk::ImageViewCreateInfo image_view_info(
      {}, image_.get(), ::vk::ImageViewType::e2D, format_, {},
      ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                  1));

  VK_TIE_RESULT(view_, ctx->graphics_context->Device().get().createImageView(
                           image_view_info));

  return {};
}

}  // namespace wren
