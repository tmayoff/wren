#include "wren/render_pass.hpp"

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "wren/context.hpp"
#include "wren/renderer.hpp"
#include "wren/utils/vulkan_errors.hpp"

namespace wren {

auto RenderPass::Create(const std::shared_ptr<Context>& ctx,
                        const std::string& name,
                        const PassResources& resources,
                        const execute_fn_t& fn)
    -> tl::expected<std::shared_ptr<RenderPass>, std::error_code> {
  auto pass =
      std::shared_ptr<RenderPass>(new RenderPass(name, resources));

  const auto& device = ctx->graphics_context->Device();
  const auto& swapchain_images =
      ctx->renderer->get_swapchain_images_views();

  const auto& shader = resources.shader;

  std::vector<vk::AttachmentDescription> attachments;

  const auto& rt = resources.render_target;
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

  vk::RenderPassCreateInfo create_info({}, attachments, subpass);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass = renderpass;

  auto size = resources.render_target->size;
  auto pipeline_res = shader->create_graphics_pipeline(
      device.get(), renderpass, size);
  if (!pipeline_res.has_value())
    return tl::make_unexpected(pipeline_res.error());

  vk::Viewport viewport(0, 0, static_cast<float>(rt->size.width),
                        static_cast<float>(rt->size.height));
  vk::Rect2D scissor({}, rt->size);
  vk::PipelineViewportStateCreateInfo viewport_state({}, viewport,
                                                     scissor);

  for (const auto& image_view : resources.render_target->image_views) {
    vk::Framebuffer fb;
    std::tie(res, fb) = device.get().createFramebuffer(
        vk::FramebufferCreateInfo{{},
                                  pass->render_pass,
                                  image_view,
                                  rt->size.width,
                                  rt->size.height,
                                  1});
    pass->framebuffers.push_back(fb);
  }

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
    const std::pair<float, float>& size) {}

void RenderPass::execute(uint32_t image_index) {
  auto& cmd = command_buffers.front();

  auto res = cmd.begin(vk::CommandBufferBeginInfo{});
  if (res != vk::Result::eSuccess) return;

  auto extent = resources.render_target->size;

  vk::ClearValue clear_value(
      vk::ClearColorValue{std::array<float, 4>{0.2, 0.2, 0.2, 1.0}});

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
}

}  // namespace wren
