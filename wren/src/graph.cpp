#include "wren/graph.hpp"

namespace wren {

auto GraphBuilder::add_pass(const std::string &name,
                            const PassResources &resources,
                            const RenderPass::execute_fn_t &fn)
    -> GraphBuilder & {
  passes.emplace_back(name, resources, fn);
  return *this;
}

auto GraphBuilder::compile() -> Graph {
  Graph graph;

  for (const auto &[name, resources, fn] : passes) {
    auto pass = RenderPass::Create(ctx, name, resources, fn).value();
    node_t n = std::make_shared<Node>(pass);

    graph.nodes.push_back(n);
  }

  return graph;
}

}  // namespace wren
