#include "wren/render_pass.hpp"

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_utils/errors.hpp>
#include <wren_vk/errors.hpp>

#include "wren/context.hpp"
#include "wren/renderer.hpp"

namespace wren {

auto RenderPass::create(const std::shared_ptr<Context>& ctx,
                        const std::string& name, const PassResources& resources,
                        const execute_fn_t& fn)
    -> expected<std::shared_ptr<RenderPass>> {
  auto pass =
      std::shared_ptr<RenderPass>(new RenderPass(ctx, name, resources, fn));

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
                          static_cast<float>(rt->size.y()));
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

void RenderPass::resize_target(const math::vec2f& new_size) {
  // TODO Resize everything

  recreate_framebuffers(ctx_->graphics_context->Device().get());
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

void RenderPass::bind_pipeline(const std::string& pipeline_name) {
  auto cmd = command_buffers_.front();

  const auto pipeline = resources_.shaders.at(pipeline_name)->get_pipeline();

  cmd.bindPipeline(::vk::PipelineBindPoint::eGraphics, pipeline);
}

RenderPass::RenderPass(const std::shared_ptr<Context>& ctx, std::string name,
                       PassResources resources, execute_fn_t fn)
    : ctx_(ctx),
      name_(std::move(name)),
      resources_(std::move(resources)),
      execute_fn_(std::move(fn)),
      target_(this->resources_.render_target) {}

}  // namespace wren
