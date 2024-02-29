#pragma once

#include <memory>
#include <string>
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
                const PassResources &resources,
                const RenderPass::execute_fn_t &fn) -> GraphBuilder &;

 private:
  std::shared_ptr<Context> ctx;
  std::vector<std::tuple<std::string, PassResources,
                         RenderPass::execute_fn_t>>
      passes;
};

}  // namespace wren
