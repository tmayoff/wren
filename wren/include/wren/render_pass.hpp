#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/image.hpp>
#include <wren_vk/shader.hpp>

#include "render_target.hpp"
#include "wren/context.hpp"

namespace wren {

// Forward declarations
struct Context;

namespace vk {
class Buffer;
}

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
                     const execute_fn_t& fn,
                     const std::optional<vk::Image>& image)
      -> expected<std::shared_ptr<RenderPass>>;

  void execute();

  template <typename T>
  void write_scratch_buffer(const ::vk::CommandBuffer& cmd, uint32_t set,
                            uint32_t binding, T data);
  [[nodiscard]] auto get_scratch_buffer(uint32_t set, uint32_t binding,
                                        size_t size) -> void*;

  auto resize_target(const math::vec2i& new_size) -> expected<void>;

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
             PassResources resources, execute_fn_t fn,
             const std::optional<vk::Image>& image);

  std::shared_ptr<Context> ctx_;

  std::string name_;
  PassResources resources_;

  execute_fn_t execute_fn_;

  ::vk::RenderPass render_pass_;

  ::vk::CommandPool command_pool_;
  std::vector<::vk::CommandBuffer> command_buffers_;

  std::optional<vk::Image> target_image_;
  std::shared_ptr<RenderTarget> target_;
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
                           resources_.shaders.at("mesh")->pipeline_layout(),
                           set, writes);
}

}  // namespace wren
