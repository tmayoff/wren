#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <wren_utils/errors.hpp>

#include "render_pass.hpp"

namespace wren {

struct Node {
  std::string name;
  std::shared_ptr<RenderPass> render_pass;
};

using node_t = std::shared_ptr<Node>;
using edge_t = std::pair<node_t, node_t>;

struct Graph {
  auto begin() { return nodes.begin(); }
  auto end() { return nodes.end(); }

  auto node_by_name(std::string const &name) const -> node_t {
    auto const node = std::ranges::find_if(
        nodes,
        [name](node_t const &node) { return name == node->name; });
    if (node != nodes.end()) return *node;

    return nullptr;
  }

  std::vector<node_t> nodes;
  std::vector<edge_t> edges;
};

class GraphBuilder {
 public:
  explicit GraphBuilder(std::shared_ptr<Context> const &ctx)
      : ctx(ctx) {}

  [[nodiscard]] auto compile() const -> expected<Graph>;

  auto add_pass(std::string const &name,
                PassResources const &resources,
                RenderPass::execute_fn_t const &fn) -> GraphBuilder &;

 private:
  std::shared_ptr<Context> ctx;
  std::vector<std::tuple<std::string, PassResources,
                         RenderPass::execute_fn_t>>
      passes;
};

}  // namespace wren
