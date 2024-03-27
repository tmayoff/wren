#pragma once

#include <queue>
#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/shader.hpp>
#include <wrenm/matrix.hpp>
#include <wrenm/vector.hpp>

namespace wren::gui {

struct Window {
  std::string name;
  wrenm::vec2f pos;
  wrenm::vec2f size;
  bool hovered = false;
};

static std::size_t const MAX_VERTICES = 10000;
static std::size_t const MAX_INDICES = MAX_VERTICES * 10;

struct Vertex {
  wrenm::vec2f pos;
  wrenm::vec4f colour;
};

struct UBO {
  wrenm::mat4f proj;
};

struct Inputs {
  wrenm::vec2f mouse_position = {0.0f, 0.0f};
  wrenm::vec2f mouse_position_rel = {0.0f, 0.0f};
  bool left_mouse = false;
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
    this->output_size = size;
  }

  auto BeginWindow(std::string const& name, wrenm::vec2f const& size)
      -> bool;
  auto EndWindow() -> bool;

  auto IO() -> Inputs& { return io; }

 private:
  void draw_quad(wrenm::vec2f const& pos, wrenm::vec2f const& size,
                 wrenm::vec4f const& colour);

  auto point_in_bounds(wrenm::vec2f const& p, wrenm::vec2f const& pos,
                       wrenm::vec2f const& size) -> bool;

  std::shared_ptr<vk::Shader> shader_;
  std::shared_ptr<wren::vk::Buffer> index_buffer;
  std::shared_ptr<wren::vk::Buffer> vertex_buffer;
  std::shared_ptr<vk::Buffer> uniform_buffer;

  VK_NS::Device device;
  VK_NS::Queue graphics_queue;
  VK_NS::CommandPool command_pool;

  VK_NS::Extent2D output_size;

  VmaAllocator allocator;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  std::queue<Window> windows_;

  Inputs io;
};

}  // namespace wren::gui
