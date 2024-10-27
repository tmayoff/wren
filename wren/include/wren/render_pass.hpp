#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <wren/vk/buffer.hpp>
#include <wren/vk/image.hpp>
#include <wren/vk/shader.hpp>

#include "render_target.hpp"
#include "wren/context.hpp"

namespace wren {

// Forward declarations
struct Context;

namespace vk {
class Buffer;
}

class PassResources {
 public:
  PassResources(std::string target_prefix)
      : target_prefix_(std::move(target_prefix)) {}

  auto add_shader(const std::string& name,
                  const std::shared_ptr<vk::Shader>& shader) -> PassResources& {
    shaders_.insert({name, shader});
    return *this;
  }

  auto add_colour_target() -> PassResources& {
    colour_target_ = true;
    return *this;
  }

  auto add_depth_target() -> PassResources& {
    depth_target_ = true;
    return *this;
  }

  auto has_colour_target() const { return colour_target_; }
  auto has_depth_target() const { return depth_target_; }

  auto target_prefix() const { return target_prefix_; }
  auto shaders() const { return shaders_; }

 private:
  std::string target_prefix_;

  bool colour_target_ = false;
  bool depth_target_ = false;

  std::unordered_map<std::string, std::shared_ptr<vk::Shader>> shaders_;
};

class RenderPass {
 public:
  using execute_fn_t = std::function<void(RenderPass&, ::vk::CommandBuffer&)>;

  static auto create(const std::shared_ptr<Context>& ctx,
                     const std::string& name, const PassResources& resources,
                     const std::shared_ptr<RenderTarget>& colour_target,
                     const std::shared_ptr<RenderTarget>& depth_target,
                     const execute_fn_t& fn)
      -> expected<std::shared_ptr<RenderPass>>;

  void execute();

  template <typename T>
  void write_scratch_buffer(const ::vk::CommandBuffer& cmd, uint32_t set,
                            uint32_t binding, T data);
  [[nodiscard]] auto get_scratch_buffer(uint32_t set, uint32_t binding,
                                        size_t size) -> void*;

  auto resize_target(const math::Vec2f& new_size) -> expected<void>;

  void on_resource_resized(const std::pair<float, float>& size);

  auto output_size() const { return size_; }

  auto resources() const { return resources_; }

  [[nodiscard]] auto get_command_buffers() const { return command_buffers_; }

  [[nodiscard]] auto get_framebuffer() const { return framebuffer_; }

  void recreate_framebuffers(const ::vk::Device& device);

  void bind_pipeline(const std::string& pipeline_name);

  [[nodiscard]] auto get() const { return render_pass_; }

 private:
  RenderPass(const std::shared_ptr<Context>& ctx, std::string name,
             PassResources resources,
             const std::shared_ptr<RenderTarget>& colour_target,
             const std::shared_ptr<RenderTarget>& depth_target,
             execute_fn_t fn);

  std::shared_ptr<Context> ctx_;

  std::string name_;
  PassResources resources_;
  std::shared_ptr<vk::Shader> last_bound_shader_;

  math::Vec2f size_{};

  execute_fn_t execute_fn_;

  ::vk::RenderPass render_pass_;

  ::vk::CommandPool command_pool_;
  std::vector<::vk::CommandBuffer> command_buffers_;

  std::shared_ptr<RenderTarget> colour_target_;
  std::shared_ptr<RenderTarget> depth_target_;

  ::vk::Framebuffer framebuffer_;

  std::map<std::pair<uint32_t, uint32_t>, std::shared_ptr<vk::Buffer>> ubos_;
};

template <typename T>
void RenderPass::write_scratch_buffer(const ::vk::CommandBuffer& cmd,
                                      uint32_t set, uint32_t binding, T data) {
  if (!ubos_.contains({set, binding})) {
    // Create buffer

    ubos_.insert(
        {{set, binding},
         vk::Buffer::create(
             ctx_->graphics_context->allocator(), sizeof(data),
             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
             VmaAllocationCreateFlagBits::
                 VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)});
  }

  auto buffer = ubos_.at({set, binding});
  buffer->set_data_raw(&data, sizeof(T));

  ::vk::DescriptorBufferInfo buffer_info(buffer->get(), 0, sizeof(T));
  std::array writes = {::vk::WriteDescriptorSet{
      {}, binding, 0, ::vk::DescriptorType::eUniformBuffer, {}, buffer_info}};

  cmd.pushDescriptorSetKHR(::vk::PipelineBindPoint::eGraphics,
                           last_bound_shader_->pipeline_layout(), set, writes);
}

}  // namespace wren
