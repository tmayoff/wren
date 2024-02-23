#include "wren/graph.hpp"

namespace wren {

auto GraphBuilder::add_pass(const std::string &name,
                            const PassResources &resources)
    -> GraphBuilder & {
  node_t n;

  auto pass = RenderPass::Create(ctx, name, resources);

  graph.nodes.push_back(n);

  return *this;
}

auto GraphBuilder::compile() -> Graph { return Graph{}; }

}  // namespace wren
