#include "wren/graph.hpp"

#include <tl/expected.hpp>
#include <wren_utils/errors.hpp>
#include <wren_vk/image.hpp>

#include "wren/context.hpp"
#include "wren/renderer.hpp"

namespace wren {

auto GraphBuilder::add_pass(const std::string &name,
                            const PassResources &resources,
                            const RenderPass::execute_fn_t &fn)
    -> GraphBuilder & {
  passes_.emplace_back(name, resources, fn);
  return *this;
}

auto GraphBuilder::compile() const -> expected<Graph> {
  Graph graph;

  // TODO Create render passes and fill missing targets

  for (const auto &[name, resources, fn] : passes_) {
    auto pass_resources = resources;

    const auto &targets = ctx_->renderer->render_targets();

    std::optional<vk::Image> image;

    if (pass_resources.render_target == nullptr) {
      if (targets.contains(pass_resources.target_name)) {
        // Resolve target names
        pass_resources.render_target =
            ctx_->renderer->render_targets().at(pass_resources.target_name);
      } else {
        // Create the target

        TRY_RESULT(auto img_target, create_target());
        std::tie(image, pass_resources.render_target) = img_target;

        pass_resources.render_target->final_layout =
            ::vk::ImageLayout::eShaderReadOnlyOptimal;
      }
    }

    TRY_RESULT(auto pass,
               RenderPass::create(ctx_, name, pass_resources, fn, image));
    node_t n = std::make_shared<Node>(name, pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

auto GraphBuilder::create_target() const
    -> expected<std::pair<vk::Image, std::shared_ptr<RenderTarget>>> {
  auto target = std::make_shared<wren::RenderTarget>(
      math::vec2i{512, 512}, ::vk::Format::eB8G8R8A8Srgb,
      ::vk::SampleCountFlagBits::e1, nullptr,
      ::vk::ImageUsageFlagBits::eColorAttachment |
          ::vk::ImageUsageFlagBits::eSampled);

  TRY_RESULT(
      auto image,
      vk::Image::create(ctx_->graphics_context->Device().get(),
                        ctx_->graphics_context->allocator(), target->format,
                        target->size, target->image_usage));

  // transition image
  TRY_RESULT(ctx_->renderer->submit_command_buffer(
      [&image](const ::vk::CommandBuffer &cmd_buf) {
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
      {}, image.get(), ::vk::ImageViewType::e2D, target->format, {},
      ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1, 0,
                                  1));
  VK_TIE_RESULT(
      target->image_view,
      ctx_->graphics_context->Device().get().createImageView(image_view_info));

  return std::make_pair(image, target);
}

}  // namespace wren
