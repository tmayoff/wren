#include "wren/graph.hpp"

namespace wren {

auto GraphBuilder::AddPass() -> GraphBuilder & {
  // TODO add node
  node_t n;

  graph.nodes.push_back(n);

  return *this;
}

} // namespace wren
