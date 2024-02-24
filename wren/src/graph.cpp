#include "wren/graph.hpp"

namespace wren {

auto GraphBuilder::add_pass(const std::string &name,
                            const PassResources &resources)
    -> GraphBuilder & {
  auto pass = RenderPass::Create(ctx, name, resources).value();
  node_t n = std::make_shared<Node>(pass);

  graph.nodes.push_back(n);

  return *this;
}

auto GraphBuilder::compile() -> Graph { return graph; }

}  // namespace wren
