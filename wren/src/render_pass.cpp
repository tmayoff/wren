#include "wren/render_pass.hpp"

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_utils/errors.hpp>
#include <wren_vk/errors.hpp>

#include "wren/context.hpp"
#include "wren/renderer.hpp"

namespace wren {

auto RenderPass::Create(std::shared_ptr<Context> const& ctx,
                        std::string const& name,
                        PassResources const& resources,
                        execute_fn_t const& fn)
    -> expected<std::shared_ptr<RenderPass>> {
  auto pass = std::shared_ptr<RenderPass>(
      new RenderPass(name, resources, fn));

  auto const& device = ctx->graphics_context->Device();
  auto const& swapchain_images =
      ctx->renderer->swapchain_images_views();

  std::vector<VK_NS::AttachmentDescription> attachments;

  auto const& rt = resources.render_target;
  VK_NS::AttachmentDescription attachment(
      {}, rt->format, rt->sample_count,
      VK_NS::AttachmentLoadOp::eClear,
      VK_NS::AttachmentStoreOp::eStore,
      VK_NS::AttachmentLoadOp::eDontCare,
      VK_NS::AttachmentStoreOp::eDontCare,
      VK_NS::ImageLayout::eUndefined,
      VK_NS::ImageLayout::ePresentSrcKHR);
  attachments.push_back(attachment);

  VK_NS::AttachmentReference attachment_ref(
      0, VK_NS::ImageLayout::eColorAttachmentOptimal);
  VK_NS::SubpassDescription subpass(
      {}, VK_NS::PipelineBindPoint::eGraphics, {}, attachment_ref);

  VK_NS::SubpassDependency dependency(
      VK_SUBPASS_EXTERNAL, 0,
      VK_NS::PipelineStageFlagBits::eColorAttachmentOutput,
      VK_NS::PipelineStageFlagBits::eColorAttachmentOutput, {},
      VK_NS::AccessFlagBits::eColorAttachmentWrite);
  VK_NS::RenderPassCreateInfo create_info({}, attachments, subpass,
                                          dependency);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass = renderpass;

  auto size = resources.render_target->size;

  for (auto [_, shader] : resources.shaders) {
    ERR_PROP_VOID(shader->create_graphics_pipeline(device.get(),
                                                   renderpass, size));
  }

  VK_NS::Viewport viewport(0, 0, static_cast<float>(rt->size.width),
                           static_cast<float>(rt->size.height));
  VK_NS::Rect2D scissor({}, rt->size);
  VK_NS::PipelineViewportStateCreateInfo viewport_state({}, viewport,
                                                        scissor);
  pass->recreate_framebuffers(device.get());

  {
    std::vector<VK_NS::CommandBuffer> cmds;
    auto [res, pool] =
        device.get().createCommandPool(VK_NS::CommandPoolCreateInfo{
            VK_NS::CommandPoolCreateFlagBits::eResetCommandBuffer,
            ctx->graphics_context->FindQueueFamilyIndices()
                .value()
                .graphics_index});

    VK_NS::CommandBufferAllocateInfo alloc_info{
        pool, VK_NS::CommandBufferLevel::ePrimary, 1};
    std::vector<VK_NS::CommandBuffer> bufs;
    std::tie(res, bufs) =
        device.get().allocateCommandBuffers(alloc_info);

    pass->command_pool = pool;
    pass->command_buffers = bufs;
  }

  return pass;
}

RenderPass::RenderPass(std::string name, PassResources resources,
                       execute_fn_t fn)
    : name(std::move(name)),
      resources(std::move(resources)),
      execute_fn(std::move(fn)),
      target(this->resources.render_target) {}

void RenderPass::on_resource_resized(
    std::pair<float, float> const& size) {}

void RenderPass::recreate_framebuffers(VK_NS::Device const& device) {
  auto const& rt = resources.render_target;

  VK_NS::FramebufferAttachmentImageInfo image_info{
      {},
      VK_NS::ImageUsageFlagBits::eColorAttachment,
      rt->size.width,
      rt->size.height,
      1,
      rt->format};
  VK_NS::FramebufferAttachmentsCreateInfo attachements(image_info);
  VK_NS::FramebufferCreateInfo create_info(
      VK_NS::FramebufferCreateFlagBits::eImageless, render_pass, 1,
      {}, rt->size.width, rt->size.height, 1, &attachements);

  auto [res, fb] = device.createFramebuffer(create_info);
  if (res != VK_NS::Result::eSuccess) {
    throw std::runtime_error("Failed to create framebuffer");
  }

  framebuffer = fb;
}

void RenderPass::execute() {
  auto& cmd = command_buffers.front();

  auto res = cmd.begin(VK_NS::CommandBufferBeginInfo{});
  if (res != VK_NS::Result::eSuccess) return;

  VK_NS::ClearValue clear_value(VK_NS::ClearColorValue{
      std::array<float, 4>{0.0, 0.0, 0.0, 1.0}});

  auto const extent = current_target_size();
  VK_NS::RenderPassAttachmentBeginInfo attachment_begin(
      target->image_view);
  VK_NS::RenderPassBeginInfo rp_begin(render_pass, framebuffer,
                                      {{}, extent}, clear_value,
                                      &attachment_begin);

  cmd.beginRenderPass(rp_begin, VK_NS::SubpassContents::eInline);

  cmd.setViewport(
      0, VK_NS::Viewport{0, 0, static_cast<float>(extent.width),
                         static_cast<float>(extent.height)});
  cmd.setScissor(0, VK_NS::Rect2D{{0, 0}, extent});

  if (execute_fn) execute_fn(*this, cmd);

  cmd.endRenderPass();

  res = cmd.end();
  if (res != VK_NS::Result::eSuccess) {
    spdlog::error("Failed to record command buffer {}",
                  make_error_code(res).message());
  }
}

void RenderPass::bind_pipeline(std::string const& pipeline_name) {
  auto cmd = command_buffers.front();

  auto const pipeline =
      resources.shaders.at(pipeline_name)->get_pipeline();

  cmd.bindPipeline(VK_NS::PipelineBindPoint::eGraphics, pipeline);
}

}  // namespace wren
