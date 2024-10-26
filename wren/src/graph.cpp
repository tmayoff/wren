#include "wren/graph.hpp"

#include <wren/utils/result.hpp>
#include <wren/vk/image.hpp>

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

    // if (pass_resources.render_target == nullptr) {
    //   if (targets.contains(pass_resources.target_name)) {
    //     // Resolve target names
    //     pass_resources.render_target =
    //         ctx_->renderer->render_targets().at(pass_resources.target_name);
    //   } else {
    //     // Create the target

    //     TRY_RESULT(pass_resources.render_target, create_target());

    //     pass_resources.render_target->final_layout =
    //         ::vk::ImageLayout::eShaderReadOnlyOptimal;
    //   }
    // }

    TRY_RESULT(auto pass, RenderPass::create(ctx_, name, pass_resources, fn));
    node_t n = std::make_shared<Node>(name, pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

auto GraphBuilder::create_target() const
    -> expected<std::shared_ptr<RenderTarget>> {
  auto target = std::make_shared<wren::RenderTarget>(
      math::vec2i{512, 512}, ::vk::Format::eB8G8R8A8Srgb,
      ::vk::SampleCountFlagBits::e1, nullptr,
      ::vk::ImageUsageFlagBits::eColorAttachment |
          ::vk::ImageUsageFlagBits::eSampled);

  {
    // Attachment colour;
    // TRY_RESULT(
    //     colour.image,
    //     vk::Image::create(ctx_->graphics_context->Device().get(),
    //                       ctx_->graphics_context->allocator(),
    //                       target->format, target->size,
    //                       target->image_usage));

    // // transition image
    // TRY_RESULT(ctx_->renderer->submit_command_buffer(
    //     [&image = colour.image](const ::vk::CommandBuffer &cmd_buf) {
    //       ::vk::ImageMemoryBarrier barrier(
    //           ::vk::AccessFlagBits::eTransferRead,
    //           ::vk::AccessFlagBits::eMemoryRead,
    //           ::vk::ImageLayout::eUndefined,
    //           ::vk::ImageLayout::eShaderReadOnlyOptimal,
    //           VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, image.get(),
    //           ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor,
    //           0,
    //                                       1, 0, 1));

    //       cmd_buf.pipelineBarrier(::vk::PipelineStageFlagBits::eTransfer,
    //                               ::vk::PipelineStageFlagBits::eTransfer,
    //                               ::vk::DependencyFlags(), {}, {}, barrier);
    //     }));

    // ::vk::ImageViewCreateInfo image_view_info(
    //     {}, colour.image.get(), ::vk::ImageViewType::e2D, target->format, {},
    //     ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1,
    //     0,
    //                                 1));
    // VK_TIE_RESULT(colour.view,
    //               ctx_->graphics_context->Device().get().createImageView(
    //                   image_view_info));
  }

  // {
  //   TRY_RESULT(
  //       auto depth_image,
  //       vk::Image::create(ctx_->graphics_context->Device().get(),
  //                         ctx_->graphics_context->allocator(),
  //                         target->format, target->size,
  //                         target->image_usage));

  //   ::vk::ImageViewCreateInfo image_view_info(
  //       {}, target->image.get(), ::vk::ImageViewType::e2D, target->format,
  //       {},
  //       ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor, 0, 1,
  //       0,
  //                                   1));
  //   VK_TIE_RESULT(target->image_view,
  //                 ctx_->graphics_context->Device().get().createImageView(
  //                     image_view_info));
  // }

  return target;
}

}  // namespace wren
