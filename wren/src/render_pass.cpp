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
                        const std::shared_ptr<RenderTarget>& colour_target,
                        const std::shared_ptr<RenderTarget>& depth_target,
                        const execute_fn_t& fn)
    -> expected<std::shared_ptr<RenderPass>> {
  auto pass = std::shared_ptr<RenderPass>(
      new RenderPass(ctx, name, resources, colour_target, depth_target, fn));

  const auto& device = ctx->graphics_context->Device();
  const auto& swapchain_images = ctx->renderer->swapchain_images_views();

  std::vector<::vk::AttachmentDescription> attachments;
  std::vector<::vk::AttachmentReference> colour_attachments;
  std::optional<::vk::AttachmentReference> depth_attachment;

  // Setup attachments
  if (colour_target != nullptr) {
    ::vk::AttachmentDescription attachment(
        {}, colour_target->format(), colour_target->sample_count(),
        ::vk::AttachmentLoadOp::eClear, ::vk::AttachmentStoreOp::eStore,
        ::vk::AttachmentLoadOp::eDontCare, ::vk::AttachmentStoreOp::eDontCare,
        ::vk::ImageLayout::eUndefined, colour_target->final_layout());
    attachments.push_back(attachment);
    colour_attachments.emplace_back(0,
                                    ::vk::ImageLayout::eColorAttachmentOptimal);
  }

  if (depth_target != nullptr) {
    // TODO Attach depth
    depth_attachment =
        ::vk::AttachmentReference{static_cast<uint32_t>(attachments.size() - 1),
                                  ::vk::ImageLayout::eDepthAttachmentOptimal};
  }

  ::vk::SubpassDescription subpass({}, ::vk::PipelineBindPoint::eGraphics, {},
                                   colour_attachments, {});
  if (depth_attachment.has_value()) {
    subpass.setPDepthStencilAttachment(&depth_attachment.value());
  }

  ::vk::SubpassDependency dependency(
      VK_SUBPASS_EXTERNAL, 0,
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput,
      ::vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
      ::vk::AccessFlagBits::eColorAttachmentWrite);
  ::vk::RenderPassCreateInfo create_info({}, attachments, subpass, dependency);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass_ = renderpass;

  math::Vec2f size{512, 512};

  // Pipelines
  for (auto [_, shader] : resources.shaders()) {
    TRY_RESULT(
        shader->create_graphics_pipeline(device.get(), renderpass, size));
  }

  ::vk::Viewport viewport(0, 0, size.x(), size.y(), 1, 0);
  ::vk::Rect2D scissor(
      {}, {static_cast<uint32_t>(size.x()), static_cast<uint32_t>(size.y())});
  ::vk::PipelineViewportStateCreateInfo viewport_state({}, viewport, scissor);
  pass->recreate_framebuffers(device.get());

  {
    // Command buffers
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

auto RenderPass::resize_target(const math::Vec2f& new_size) -> expected<void> {
  if (colour_target_ != nullptr) {
    colour_target_->resize(ctx_, new_size);
  }

  if (depth_target_ != nullptr) {
    depth_target_->resize(ctx_, new_size);
  }

  // for (const auto rt : resources_.render_targets) {
  //   VK_TIE_RESULT(rt->view,
  //                 ctx_->graphics_context->Device().get().createImageView(
  //                     image_view_info));
  recreate_framebuffers(ctx_->graphics_context->Device().get());
  // }

  return {};
}

void RenderPass::on_resource_resized(const std::pair<float, float>& size) {
  // Recreate framebuffer passed on new
}

void RenderPass::recreate_framebuffers(const ::vk::Device& device) {
  if (colour_target_ != nullptr) {
    std::vector<::vk::ImageView> attachements = {colour_target_->view()};
    if (depth_target_ != nullptr) attachements.push_back(depth_target_->view());

    ::vk::FramebufferCreateInfo create_info(
        ::vk::FramebufferCreateFlagBits::eImageless, render_pass_, attachements,
        static_cast<uint32_t>(colour_target_->size().x()),
        static_cast<uint32_t>(colour_target_->size().y()), 1);

    auto [res, fb] = device.createFramebuffer(create_info);
    if (res != ::vk::Result::eSuccess) {
      throw std::runtime_error("Failed to create framebuffer");
    }

    framebuffer_ = fb;
  }
}

void RenderPass::execute() {
  auto& cmd = command_buffers_.front();

  auto res = cmd.begin(::vk::CommandBufferBeginInfo{});
  if (res != ::vk::Result::eSuccess) return;

  ::vk::ClearValue clear_value(
      ::vk::ClearColorValue{std::array<float, 4>{0.0, 0.0, 0.0, 1.0}});

  const auto extent = ::vk::Extent2D{static_cast<uint32_t>(output_size().x()),
                                     static_cast<uint32_t>(output_size().y())};

  const auto view = colour_target_->view();
  ::vk::RenderPassAttachmentBeginInfo attachment_begin(view);
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

  const auto pipeline = resources_.shaders().at(pipeline_name)->get_pipeline();

  cmd.bindPipeline(::vk::PipelineBindPoint::eGraphics, pipeline);
  last_bound_shader_ = resources_.shaders().at(pipeline_name);
}

RenderPass::RenderPass(const std::shared_ptr<Context>& ctx, std::string name,
                       PassResources resources,
                       const std::shared_ptr<RenderTarget>& colour_target,
                       const std::shared_ptr<RenderTarget>& depth_target,
                       execute_fn_t fn)
    : ctx_(ctx),
      name_(std::move(name)),
      resources_(std::move(resources)),
      execute_fn_(std::move(fn)),
      colour_target_(colour_target),
      depth_target_(depth_target) {}

}  // namespace wren
