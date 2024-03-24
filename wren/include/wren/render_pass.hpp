#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <wren_vk/shader.hpp>

#include "render_target.hpp"

namespace wren {

struct Context;

struct PassResources {
  std::map<std::string, std::shared_ptr<vk::Shader>> shaders;
  std::string target_name;
  std::shared_ptr<RenderTarget> render_target;
};

class RenderPass {
 public:
  using execute_fn_t =
      std::function<void(RenderPass&, VK_NS::CommandBuffer&)>;

  static auto Create(std::shared_ptr<Context> const& ctx,
                     std::string const& name,
                     PassResources const& resources,
                     execute_fn_t const& fn)
      -> tl::expected<std::shared_ptr<RenderPass>, std::error_code>;

  void execute();

  void on_resource_resized(std::pair<float, float> const& size);

  [[nodiscard]] auto get_command_buffers() const {
    return command_buffers;
  }

  [[nodiscard]] auto get_framebuffer() const { return framebuffer; }

  void recreate_framebuffers(VK_NS::Device const& device);

  void bind_pipeline(std::string const& pipeline_name);

 private:
  RenderPass(std::string name, PassResources resources,
             execute_fn_t fn);

  std::string name;
  PassResources resources;

  execute_fn_t execute_fn;

  VK_NS::RenderPass render_pass;

  VK_NS::CommandPool command_pool;
  std::vector<VK_NS::CommandBuffer> command_buffers;

  std::shared_ptr<RenderTarget> target;
  VK_NS::Framebuffer framebuffer;
};

}  // namespace wren
