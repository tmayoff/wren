#pragma once

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <wren/utils/result.hpp>
#include <wren/vk/image.hpp>

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

  [[nodiscard]] auto node_by_name(const std::string &name) const -> node_t {
    const auto node = std::ranges::find_if(
        nodes, [name](const node_t &node) { return name == node->name; });
    if (node != nodes.end()) return *node;

    return nullptr;
  }

  std::vector<node_t> nodes;
  std::vector<edge_t> edges;
};

class GraphBuilder {
 public:
  explicit GraphBuilder(const std::shared_ptr<Context> &ctx) : ctx_(ctx) {}

  /**
    @brief This calls compile() and then assigns the graph to the renderer
  */
  [[nodiscard]] auto build() const -> expected<void>;

  /**
    @brief Compiles the builder into an actual graph, it creates missing resources for targets and binds targets to the swapchain
  */
  [[nodiscard]] auto compile() const -> expected<Graph>;

  auto add_pass(const std::string &name, const PassResources &resources,
                const RenderPass::execute_fn_t &fn) -> GraphBuilder &;

 private:
  [[nodiscard]] auto create_target() const
      -> expected<std::shared_ptr<RenderTarget>>;

  std::shared_ptr<Context> ctx_;
  std::vector<std::tuple<std::string, PassResources, RenderPass::execute_fn_t>>
      passes_;
};

}  // namespace wren
