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
  std::shared_ptr<RenderTarget> render_target;
};

class RenderPass {
 public:
  using execute_fn_t = std::function<void(vk::CommandBuffer&)>;

  static auto Create(const std::shared_ptr<Context>& ctx,
                     const std::string& name,
                     const PassResources& resources,
                     const execute_fn_t& fn)
      -> tl::expected<std::shared_ptr<RenderPass>, std::error_code>;

  void execute(uint32_t image_index);

  void on_resource_resized(const std::pair<float, float>& size);

  [[nodiscard]] auto get_command_buffers() const {
    return command_buffers;
  }

  [[nodiscard]] auto get_framebuffers() const { return framebuffers; }

  void recreate_framebuffers(const vk::Device& device);

 private:
  RenderPass(std::string name, PassResources resources,
             execute_fn_t fn)
      : name(std::move(name)),
        resources(std::move(resources)),
        execute_fn(std::move(fn)) {}

  std::string name;
  PassResources resources;

  execute_fn_t execute_fn;

  vk::RenderPass render_pass;

  vk::CommandPool command_pool;
  std::vector<vk::CommandBuffer> command_buffers;

  // std::shared_ptr<RenderTarget> targets;
  std::vector<vk::Framebuffer> framebuffers;
};

}  // namespace wren
