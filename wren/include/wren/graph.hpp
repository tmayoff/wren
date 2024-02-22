#pragma once

#include <memory>
#include <vector>

#include "render_pass.hpp"
#include "shader.hpp"

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
  explicit GraphBuilder(const vk::Device &device) : device(device) {}

  auto compile() -> Graph;

  auto add_pass(const std::string &name,
                const PassResources &resources) -> GraphBuilder &;

 private:
  vk::Device device;
  Graph graph;
};

}  // namespace wren
