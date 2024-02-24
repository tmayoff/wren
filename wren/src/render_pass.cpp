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
                        const PassResources& resources)
    -> tl::expected<std::shared_ptr<RenderPass>, std::error_code> {
  auto pass =
      std::shared_ptr<RenderPass>(new RenderPass(name, resources));

  const auto& device = ctx->graphics_context->Device();
  const auto& swapchain_images =
      ctx->renderer->get_swapchain_images_views();

  const auto& shader = resources.shader;
  auto pipeline_create_info = shader->reflect_graphics_pipeline();

  const auto& rt = resources.render_targets.front();
  vk::Viewport viewport(0, 0, static_cast<float>(rt.size.width),
                        static_cast<float>(rt.size.height));
  vk::Rect2D scissor({}, rt.size);
  vk::PipelineViewportStateCreateInfo viewport_state({}, viewport,
                                                     scissor);
  pipeline_create_info.setPViewportState(&viewport_state);

  std::vector<vk::AttachmentDescription> attachments;
  attachments.reserve(resources.render_targets.size());

  for (const auto& rt : resources.render_targets) {
    vk::AttachmentDescription attachment(
        {}, rt.format, rt.sample_count, vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR);
    attachments.push_back(attachment);
  }

  vk::AttachmentReference attachment_ref(
      0, vk::ImageLayout::eColorAttachmentOptimal);
  vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics,
                                 {}, attachment_ref);

  vk::RenderPassCreateInfo create_info({}, attachments, subpass);

  auto [res, renderpass] = device.get().createRenderPass(create_info);
  pass->render_pass = renderpass;

  pipeline_create_info.setRenderPass(renderpass);
  vk::Pipeline pipeline;
  std::tie(res, pipeline) =
      device.get().createGraphicsPipeline({}, pipeline_create_info);

  pass->pipeline = pipeline;

  for (const auto& rt : resources.render_targets) {
    std::vector<vk::Framebuffer> framebuffers;
    for (const auto& image : swapchain_images) {
      auto [res, framebuffer] = device.get().createFramebuffer(
          vk::FramebufferCreateInfo{{},
                                    renderpass,
                                    image,
                                    rt.size.width,
                                    rt.size.height,
                                    1});
      if (res != vk::Result::eSuccess) {
        return tl::make_unexpected(make_error_code(res));
      }
      framebuffers.push_back(framebuffer);
    }
    pass->framebuffers.push_back(framebuffers);
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

void RenderPass::execute(uint32_t image_index) {
  const auto& cmd = command_buffers.front();

  auto res = cmd.begin(vk::CommandBufferBeginInfo{});
  if (res != vk::Result::eSuccess) return;

  auto extent = resources.render_targets.front().size;

  vk::ClearValue clear_value(vk::ClearColorValue{
      std::array<float, 4>{0.5f, 0.2f, 0.0, 1.0}});

  vk::RenderPassBeginInfo rp_begin(
      render_pass, framebuffers.front().at(image_index), {{}, extent},
      clear_value);

  cmd.beginRenderPass(rp_begin, vk::SubpassContents::eInline);

  cmd.endRenderPass();

  cmd.end();
}

}  // namespace wren
