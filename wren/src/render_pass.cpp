#include "wren/render_pass.hpp"

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "utils/errors.hpp"
#include "wren/context.hpp"
#include "wren/renderer.hpp"
#include "wren/utils/vulkan_errors.hpp"

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
      ctx->renderer->get_swapchain_images_views();

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

void RenderPass::on_resource_resized(
    std::pair<float, float> const& size) {}

void RenderPass::recreate_framebuffers(vk::Device const& device) {
  vk::Result res = vk::Result::eSuccess;

  framebuffers.clear();

  auto const& rt = resources.render_target;
  for (auto const& image_view :
       resources.render_target->image_views) {
    vk::Framebuffer fb;
    std::tie(res, fb) = device.createFramebuffer(
        vk::FramebufferCreateInfo{{},
                                  render_pass,
                                  image_view,
                                  rt->size.width,
                                  rt->size.height,
                                  1});
    framebuffers.push_back(fb);
  }
}

void RenderPass::execute(uint32_t image_index) {
  auto& cmd = command_buffers.front();

  auto res = cmd.begin(vk::CommandBufferBeginInfo{});
  if (res != vk::Result::eSuccess) return;

  auto const extent = resources.render_target->size;

  vk::ClearValue clear_value(
      vk::ClearColorValue{std::array<float, 4>{0.0, 0.0, 0.0, 1.0}});

  vk::RenderPassBeginInfo rp_begin(render_pass,
                                   framebuffers.at(image_index),
                                   {{}, extent}, clear_value);

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
