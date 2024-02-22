#include "wren/render_pass.hpp"

#include <tl/expected.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren {

auto RenderPass::Create(const vk::Device& device,
                        const std::string& name,
                        const PassResources& resources)
    -> tl::expected<RenderPass, std::error_code> {
  RenderPass pass(name, resources);

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

  auto [res, renderpass] = device.createRenderPass(create_info);
  return pass;
}

}  // namespace wren
