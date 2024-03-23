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
    -> tl::expected<std::shared_ptr<RenderPass>, std::error_code> {
  auto pass = std::shared_ptr<RenderPass>(
      new RenderPass(name, resources, fn));

  auto const& device = ctx->graphics_context->Device();
  auto const& swapchain_images =
      ctx->renderer->swapchain_images_views();

  auto const& shader = resources.shader;

  std::vector<vk::AttachmentDescription> attachments;

  auto const& rt = resources.render_target;
  vk::AttachmentDescription attachment(
      {}, rt->format, rt->sample_count, vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eDontCare,
      vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
      vk::ImageLayout::ePresentSrcKHR);
  attachments.push_back(attachment);

  vk::AttachmentReference attachment_ref(
      0, vk::ImageLayout::eColorAttachmentOptimal);
  vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics,
                                 {}, attachment_ref);

  vk::SubpassDependency dependency(
      VK_SUBPASS_EXTERNAL, 0,
      vk::PipelineStageFlagBits::eColorAttachmentOutput,
      vk::PipelineStageFlagBits::eColorAttachmentOutput, {},
      vk::AccessFlagBits::eColorAttachmentWrite);
  vk::RenderPassCreateInfo create_info({}, attachments, subpass,
                                       dependency);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass = renderpass;

  auto size = resources.render_target->size;
  ERR_PROP_VOID(shader->create_graphics_pipeline(device.get(),
                                                 renderpass, size));

  vk::Viewport viewport(0, 0, static_cast<float>(rt->size.width),
                        static_cast<float>(rt->size.height));
  vk::Rect2D scissor({}, rt->size);
  vk::PipelineViewportStateCreateInfo viewport_state({}, viewport,
                                                     scissor);
  pass->recreate_framebuffers(device.get());

  {
    std::vector<vk::CommandBuffer> cmds;
    auto [res, pool] =
        device.get().createCommandPool(vk::CommandPoolCreateInfo{
            vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
            ctx->graphics_context->FindQueueFamilyIndices()
                .value()
                .graphics_index});

    vk::CommandBufferAllocateInfo alloc_info{
        pool, vk::CommandBufferLevel::ePrimary, 1};
    std::vector<vk::CommandBuffer> bufs;
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

void RenderPass::recreate_framebuffers(vk::Device const& device) {
  auto const& rt = resources.render_target;

  vk::FramebufferAttachmentImageInfo image_info{
      {},
      vk::ImageUsageFlagBits::eColorAttachment,
      rt->size.width,
      rt->size.height,
      1,
      rt->format};
  vk::FramebufferAttachmentsCreateInfo attachements(image_info);
  vk::FramebufferCreateInfo create_info(
      vk::FramebufferCreateFlagBits::eImageless, render_pass, 1, {},
      rt->size.width, rt->size.height, 1, &attachements);

  auto [res, fb] = device.createFramebuffer(create_info);
  if (res != vk::Result::eSuccess) {
    throw std::runtime_error("Failed to create framebuffer");
  }

  framebuffer = fb;
}

void RenderPass::execute() {
  auto& cmd = command_buffers.front();

  auto res = cmd.begin(vk::CommandBufferBeginInfo{});
  if (res != vk::Result::eSuccess) return;

  auto const extent = resources.render_target->size;

  vk::ClearValue clear_value(
      vk::ClearColorValue{std::array<float, 4>{0.0, 0.0, 0.0, 1.0}});

  vk::RenderPassAttachmentBeginInfo attachment_begin(
      target->image_view);
  vk::RenderPassBeginInfo rp_begin(render_pass, framebuffer,
                                   {{}, extent}, clear_value,
                                   &attachment_begin);

  cmd.beginRenderPass(rp_begin, vk::SubpassContents::eInline);

  cmd.bindPipeline(vk::PipelineBindPoint::eGraphics,
                   resources.shader->get_pipeline());

  cmd.setViewport(0,
                  vk::Viewport{0, 0, static_cast<float>(extent.width),
                               static_cast<float>(extent.height)});
  cmd.setScissor(0, vk::Rect2D{{0, 0}, extent});

  if (execute_fn) execute_fn(cmd);

  cmd.endRenderPass();

  res = cmd.end();
  if (res != vk::Result::eSuccess) {
    spdlog::error("Failed to record command buffer {}",
                  make_error_code(res).message());
  }
}

}  // namespace wren
