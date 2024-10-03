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
  std::unordered_map<std::string, std::shared_ptr<vk::Shader>> shaders;
  std::string target_name;
  std::shared_ptr<RenderTarget> render_target;
};

class RenderPass {
 public:
  using execute_fn_t = std::function<void(RenderPass&, ::vk::CommandBuffer&)>;

  static auto create(const std::shared_ptr<Context>& ctx,
                     const std::string& name, const PassResources& resources,
                     const execute_fn_t& fn)
      -> expected<std::shared_ptr<RenderPass>>;

  void execute();

  void resize_target(const math::vec2f& new_size);

  void on_resource_resized(const std::pair<float, float>& size);

  auto current_target_size() { return resources_.render_target->size; }

  [[nodiscard]] auto get_command_buffers() const { return command_buffers_; }

  [[nodiscard]] auto get_framebuffer() const { return framebuffer_; }

  void recreate_framebuffers(const ::vk::Device& device);

  void bind_pipeline(const std::string& pipeline_name);

  [[nodiscard]] auto get() const { return render_pass_; }

  [[nodiscard]] auto target() const { return target_; }

 private:
  RenderPass(const std::shared_ptr<Context>& ctx, std::string name,
             PassResources resources, execute_fn_t fn);

  std::shared_ptr<Context> ctx_;

  std::string name_;
  PassResources resources_;

  execute_fn_t execute_fn_;

  ::vk::RenderPass render_pass_;

  ::vk::CommandPool command_pool_;
  std::vector<::vk::CommandBuffer> command_buffers_;

  std::optional<::vk::Image> target_image_;
  std::shared_ptr<RenderTarget> target_;
  ::vk::Framebuffer framebuffer_;
};

}  // namespace wren
