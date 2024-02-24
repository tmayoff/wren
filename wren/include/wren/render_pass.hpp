#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

#include "shader.hpp"

namespace wren {

struct Context;

struct RenderTarget {
  vk::Extent2D size;
  vk::Format format;
  vk::SampleCountFlagBits sample_count;
};

struct PassResources {
  std::shared_ptr<Shader> shader;
  std::vector<RenderTarget> render_targets;
};

class RenderPass {
 public:
  static auto Create(const std::shared_ptr<Context>& ctx,
                     const std::string& name,
                     const PassResources& resources)
      -> tl::expected<std::shared_ptr<RenderPass>, std::error_code>;

  void execute(uint32_t image_index);

  [[nodiscard]] auto get_command_buffers() const {
    return command_buffers;
  }

 private:
  RenderPass(std::string name, PassResources resources)
      : name(std::move(name)), resources(std::move(resources)) {}

  std::string name;
  PassResources resources;
  vk::Pipeline pipeline;

  vk::RenderPass render_pass;

  vk::CommandPool command_pool;
  std::vector<vk::CommandBuffer> command_buffers;
  std::vector<std::vector<vk::Framebuffer>> framebuffers;
};

}  // namespace wren
