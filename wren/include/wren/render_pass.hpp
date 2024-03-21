#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

#include "render_target.hpp"
#include "shader.hpp"

namespace wren {

struct Context;

struct PassResources {
  std::shared_ptr<Shader> shader;
  std::string target_name;
  std::shared_ptr<RenderTarget> render_target;
};

class RenderPass {
 public:
  using execute_fn_t = std::function<void(vk::CommandBuffer&)>;

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

  void recreate_framebuffers(vk::Device const& device);

 private:
  RenderPass(std::string name, PassResources resources,
             execute_fn_t fn);

  std::string name;
  PassResources resources;

  execute_fn_t execute_fn;

  vk::RenderPass render_pass;

  vk::CommandPool command_pool;
  std::vector<vk::CommandBuffer> command_buffers;

  std::shared_ptr<RenderTarget> target;
  vk::Framebuffer framebuffer;
};

}  // namespace wren
