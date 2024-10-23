#include "wren/render_pass.hpp"

#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren/utils/result.hpp>
#include <wren/vk/result.hpp>

#include "wren/context.hpp"
#include "wren/renderer.hpp"

namespace wren {

auto RenderPass::create(const std::shared_ptr<Context>& ctx,
                        const std::string& name, const PassResources& resources,
                        const execute_fn_t& fn,
                        const std::optional<vk::Image>& image)
    -> expected<std::shared_ptr<RenderPass>> {
  auto pass = std::shared_ptr<RenderPass>(
      new RenderPass(ctx, name, resources, fn, image));

  const auto& device = ctx->graphics_context->Device();
  const auto& swapchain_images = ctx->renderer->swapchain_images_views();

  std::vector<::vk::AttachmentDescription> attachments;

  const auto& rt = resources.render_target;

  ::vk::AttachmentDescription attachment(
      {}, rt->format, rt->sample_count, ::vk::AttachmentLoadOp::eClear,
      ::vk::AttachmentStoreOp::eStore, ::vk::AttachmentLoadOp::eDontCare,
      ::vk::AttachmentStoreOp::eDontCare, ::vk::ImageLayout::eUndefined,
      rt->final_layout);
  attachments.push_back(attachment);

  ::vk::AttachmentReference attachment_ref(
      0, ::vk::ImageLayout::eColorAttachmentOptimal);
  ::vk::SubpassDescription subpass({}, ::vk::PipelineBindPoint::eGraphics, {},
                                   attachment_ref);

  ::vk::SubpassDependency dependency(
      VK_SUBPASS_EXTERNAL, 0,
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput,
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
      ::vk::AccessFlagBits::eColorAttachmentWrite);
  ::vk::RenderPassCreateInfo create_info({}, attachments, subpass, dependency);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass_ = renderpass;

  auto size = resources.render_target->size;

  for (auto [_, shader] : resources.shaders) {
    TRY_RESULT(
        shader->create_graphics_pipeline(device.get(), renderpass, size));
  }

  ::vk::Viewport viewport(0, 0, static_cast<float>(rt->size.x()),
                          static_cast<float>(rt->size.y()), 1, 0);
  ::vk::Rect2D scissor({}, {static_cast<uint32_t>(rt->size.x()),
                            static_cast<uint32_t>(rt->size.y())});
  ::vk::PipelineViewportStateCreateInfo viewport_state({}, viewport, scissor);
  pass->recreate_framebuffers(device.get());

  {
    std::vector<::vk::CommandBuffer> cmds;
    auto [res, pool] =
        device.get().createCommandPool(::vk::CommandPoolCreateInfo{
            ::vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            ctx->graphics_context->FindQueueFamilyIndices()
                .value()
                .graphics_index});

    ::vk::CommandBufferAllocateInfo alloc_info{
        pool, ::vk::CommandBufferLevel::ePrimary, 1};
    std::vector<::vk::CommandBuffer> bufs;
    std::tie(res, bufs) = device.get().allocateCommandBuffers(alloc_info);

    pass->command_pool_ = pool;
    pass->command_buffers_ = bufs;
  }

  return pass;
}

auto RenderPass::resize_target(const math::vec2i& new_size) -> expected<void> {
  target_->size = new_size;

  // Delete image
  target_image_.reset();

  // Create a new image

  TRY_RESULT(
      target_image_,
      vk::Image::create(ctx_->graphics_context->Device().get(),
                        ctx_->graphics_context->allocator(), target_->format,
                        target_->size, target_->image_usage));

  // transition image
  TRY_RESULT(ctx_->renderer->submit_command_buffer(
      [this](const ::vk::CommandBuffer& cmd_buf) {
        ::vk::ImageMemoryBarrier barrier(
            ::vk::AccessFlagBits::eTransferRead,
            ::vk::AccessFlagBits::eMemoryRead, ::vk::ImageLayout::eUndefined,
            ::vk::ImageLayout::eShaderReadOnlyOptimal, VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED, target_image_->get(),
            ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1,
                                        0, 1));

        cmd_buf.pipelineBarrier(::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::PipelineStageFlagBits::eTransfer,
                                ::vk::DependencyFlags(), {}, {}, barrier);
      }));

  ::vk::ImageViewCreateInfo image_view_info(
      {}, target_image_->get(), ::vk::ImageViewType::e2D, target_->format, {},
      ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                  1));
  VK_TIE_RESULT(
      target_->image_view,
      ctx_->graphics_context->Device().get().createImageView(image_view_info));
  recreate_framebuffers(ctx_->graphics_context->Device().get());

  return {};
}

void RenderPass::on_resource_resized(const std::pair<float, float>& size) {
  // Recreate framebuffer passed on new
}

void RenderPass::recreate_framebuffers(const ::vk::Device& device) {
  const auto& rt = resources_.render_target;

  ::vk::FramebufferAttachmentImageInfo image_info{
      {},
      target_->image_usage,
      static_cast<uint32_t>(rt->size.x()),
      static_cast<uint32_t>(rt->size.y()),
      1,
      rt->format};
  ::vk::FramebufferAttachmentsCreateInfo attachements(image_info);
  ::vk::FramebufferCreateInfo create_info(
      ::vk::FramebufferCreateFlagBits::eImageless, render_pass_, 1, {},
      rt->size.x(), rt->size.y(), 1, &attachements);

  auto [res, fb] = device.createFramebuffer(create_info);
  if (res != ::vk::Result::eSuccess) {
    throw std::runtime_error("Failed to create framebuffer");
  }

  framebuffer_ = fb;
}

void RenderPass::execute() {
  auto& cmd = command_buffers_.front();

  auto res = cmd.begin(::vk::CommandBufferBeginInfo{});
  if (res != ::vk::Result::eSuccess) return;

  ::vk::ClearValue clear_value(
      ::vk::ClearColorValue{std::array<float, 4>{0.0, 0.0, 0.0, 1.0}});

  const auto extent =
      ::vk::Extent2D{static_cast<uint32_t>(current_target_size().x()),
                     static_cast<uint32_t>(current_target_size().y())};
  ::vk::RenderPassAttachmentBeginInfo attachment_begin(target_->image_view);
  ::vk::RenderPassBeginInfo rp_begin(render_pass_, framebuffer_, {{}, extent},
                                     clear_value, &attachment_begin);

  cmd.beginRenderPass(rp_begin, ::vk::SubpassContents::eInline);

  cmd.setViewport(0, ::vk::Viewport{0, 0, static_cast<float>(extent.width),
                                    static_cast<float>(extent.height)});
  cmd.setScissor(0, ::vk::Rect2D{{0, 0}, extent});

  if (execute_fn_) execute_fn_(*this, cmd);

  cmd.endRenderPass();

  res = cmd.end();
  if (res != ::vk::Result::eSuccess) {
    spdlog::error("Failed to record command buffer {}",
                  make_error_code(res).message());
  }
}

auto RenderPass::get_scratch_buffer(uint32_t set, uint32_t binding, size_t size)
    -> void* {
  auto buf = vk::Buffer::create(
      ctx_->graphics_context->allocator(), size,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
      VmaAllocationCreateFlagBits::
          VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

  return buf->map();
}

void RenderPass::bind_pipeline(const std::string& pipeline_name) {
  auto cmd = command_buffers_.front();

  const auto pipeline = resources_.shaders.at(pipeline_name)->get_pipeline();

  cmd.bindPipeline(::vk::PipelineBindPoint::eGraphics, pipeline);
  last_bound_shader_ = resources_.shaders.at(pipeline_name);
}

RenderPass::RenderPass(const std::shared_ptr<Context>& ctx, std::string name,
                       PassResources resources, execute_fn_t fn,
                       const std::optional<vk::Image>& image)
    : ctx_(ctx),
      name_(std::move(name)),
      resources_(std::move(resources)),
      execute_fn_(std::move(fn)),
      target_image_(image),
      target_(this->resources_.render_target) {}

}  // namespace wren
