#include "wren/graph.hpp"

#include <tl/expected.hpp>
#include <wren_utils/errors.hpp>

#include "context.hpp"
#include "renderer.hpp"

namespace wren {

auto GraphBuilder::add_pass(std::string const &name,
                            PassResources const &resources,
                            RenderPass::execute_fn_t const &fn)
    -> GraphBuilder & {
  passes.emplace_back(name, resources, fn);
  return *this;
}

auto GraphBuilder::compile() const -> expected<Graph> {
  Graph graph;

  // TODO Create render passes and fill missing targets

  for (auto const &[name, resources, fn] : passes) {
    auto pass_resources = resources;
    if (pass_resources.render_target == nullptr) {
      // Resolve target names
      pass_resources.render_target =
          ctx->renderer->render_targets().at(
              pass_resources.target_name);
    }

    TRY_RESULT(auto pass,
             RenderPass::Create(ctx, name, pass_resources, fn));
    node_t n = std::make_shared<Node>(pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

}  // namespace wren
