#include "wren/graph.hpp"

#include <wren/utils/result.hpp>
#include <wren/vk/image.hpp>

#include "render_target.hpp"
#include "renderer.hpp"

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

  for (const auto &[name, resources, fn] : passes_) {
    const auto &pass_resources = resources;

    std::shared_ptr<RenderTarget> colour_target;
    std::shared_ptr<RenderTarget> depth_target;

    if (pass_resources.target_prefix() == "swapchain_target") {
      // Get the create targets from the renderer
      // Resolve target names
      colour_target =
          ctx_->renderer->render_targets().at(pass_resources.target_prefix());
    }

    const auto &targets = ctx_->renderer->render_targets();

    if (colour_target == nullptr && pass_resources.has_colour_target()) {
      TRY_RESULT(colour_target, RenderTarget::create(ctx_));
      colour_target->final_layout(::vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    if (depth_target == nullptr && pass_resources.has_depth_target()) {
      TRY_RESULT(depth_target, RenderTarget::create_depth(ctx_));
    }

    TRY_RESULT(auto pass, RenderPass::create(ctx_, name, pass_resources,
                                             colour_target, depth_target, fn));
    node_t n = std::make_shared<Node>(name, pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

}  // namespace wren
