#pragma once

#include <memory>
#include <vector>

namespace wren {

class Node {};

using node_t = std::shared_ptr<Node>;
using edge_t = std::pair<node_t, node_t>;

struct Graph {
  std::vector<node_t> nodes;
  std::vector<edge_t> edges;
};

class GraphBuilder {
public:
  auto AddPass() -> GraphBuilder &;

private:
  Graph graph;
};

} // namespace wren
