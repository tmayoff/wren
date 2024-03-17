#include "wren/graph.hpp"

#include <tl/expected.hpp>
#include <utils/errors.hpp>

namespace wren {

auto GraphBuilder::add_pass(std::string const &name,
                            PassResources const &resources,
                            RenderPass::execute_fn_t const &fn)
    -> GraphBuilder & {
  passes.emplace_back(name, resources, fn);
  return *this;
}

auto GraphBuilder::compile() -> tl::expected<Graph, std::error_code> {
  Graph graph;

  for (auto const &[name, resources, fn] : passes) {
    ERR_PROP(auto pass, RenderPass::Create(ctx, name, resources, fn));
    node_t n = std::make_shared<Node>(pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

}  // namespace wren
