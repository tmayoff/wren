#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

#include "render_pass.hpp"
#include "shader.hpp"

namespace wren {

struct Node {
  std::shared_ptr<RenderPass> render_pass;
};

using node_t = std::shared_ptr<Node>;
using edge_t = std::pair<node_t, node_t>;

struct Graph {
  auto begin() { return nodes.begin(); }
  auto end() { return nodes.end(); }

  std::vector<node_t> nodes;
  std::vector<edge_t> edges;
};

class GraphBuilder {
 public:
  explicit GraphBuilder(const std::shared_ptr<Context> &ctx)
      : ctx(ctx) {}

  auto compile() -> Graph;

  auto add_pass(const std::string &name,
                const PassResources &resources) -> GraphBuilder &;

 private:
  Graph graph;
  std::shared_ptr<Context> ctx;
};

}  // namespace wren
