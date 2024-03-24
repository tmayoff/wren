#pragma once

#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/shader.hpp>
#include <wrenm/matrix.hpp>
#include <wrenm/vector.hpp>

namespace wren::gui {

static std::size_t const MAX_VERTICES = 10000;
static std::size_t const MAX_INDICES = MAX_VERTICES * 10;

struct Vertex {
  wrenm::vec2f pos;
  wrenm::vec4f colour;
};

struct UBO {
  wrenm::mat4f proj;
};

class Instance {
 public:
  Instance(std::shared_ptr<vk::Shader> const& shader,
           VK_NS::Device const& device, VmaAllocator const& allocator,
           VK_NS::CommandPool const& command_pool,
           VK_NS::Queue const& graphics_queue);

  [[nodiscard]] auto shader() const { return shader_; }

  void bind(VK_NS::CommandBuffer const& cmd);
  void flush(VK_NS::CommandBuffer const& cmd);

  void resize_viewport(VK_NS::Extent2D const& size) {
    this->size = size;
  }

  auto BeginWindow() -> bool;
  auto EndWindow() -> bool;

  auto mouse_position(float x, float y) { mouse_position_ = {x, y}; }
  auto mouse_position() { return mouse_position_; }

 private:
  void draw_quad();

  std::shared_ptr<vk::Shader> shader_;
  std::shared_ptr<wren::vk::Buffer> index_buffer;
  std::shared_ptr<wren::vk::Buffer> vertex_buffer;
  std::shared_ptr<vk::Buffer> uniform_buffer;

  VK_NS::Device device;
  VK_NS::Queue graphics_queue;
  VK_NS::CommandPool command_pool;

  VK_NS::Extent2D size;

  VmaAllocator allocator;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  wrenm::vec2f mouse_position_;
};

}  // namespace wren::gui
