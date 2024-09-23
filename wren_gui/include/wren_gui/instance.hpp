#pragma once

#include <queue>
#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/shader.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/vector.hpp>

namespace wren::gui {

struct Window {
  std::string name;
  wren::math::vec2f pos;
  wren::math::vec2f size;

  // IO

  bool hovered = false;
  bool selected = false;
  wren::math::vec2f mouse_offset;
};

static std::size_t const MAX_VERTICES = 10000;
static std::size_t const MAX_INDICES = MAX_VERTICES * 10;

struct Vertex {
  wren::math::vec2f pos;
  wren::math::vec4f colour;
};

struct UBO {
  wren::math::mat4f proj;
};

struct Inputs {
  wren::math::vec2f mouse_position = {0.0f, 0.0f};
  wren::math::vec2f mouse_position_rel = {0.0f, 0.0f};
  bool left_mouse = false;

  // Left mouse down this frame
  bool left_mouse_down = false;
};

class Instance {
 public:
  Instance(std::shared_ptr<vk::Shader> const& shader,
           ::vk::Device const& device, VmaAllocator const& allocator,
           ::vk::CommandPool const& command_pool,
           ::vk::Queue const& graphics_queue);

  [[nodiscard]] auto shader() const { return shader_; }

  void bind(::vk::CommandBuffer const& cmd);
  void flush(::vk::CommandBuffer const& cmd);

  void resize_viewport(::vk::Extent2D const& size) {
    this->output_size = size;
  }

  void Begin();
  void End();

  auto BeginWindow(std::string const& name, wren::math::vec2f const& size)
      -> bool;
  void EndWindow();

  auto IO() -> Inputs& { return io; }

 private:
  void draw_quad(wren::math::vec2f const& pos, wren::math::vec2f const& size,
                 wren::math::vec4f const& colour);

  auto point_in_bounds(wren::math::vec2f const& p, wren::math::vec2f const& pos,
                       wren::math::vec2f const& size) -> bool;

  std::shared_ptr<vk::Shader> shader_;
  std::shared_ptr<wren::vk::Buffer> index_buffer;
  std::shared_ptr<wren::vk::Buffer> vertex_buffer;
  std::shared_ptr<vk::Buffer> uniform_buffer;

  ::vk::Device device;
  ::vk::Queue graphics_queue;
  ::vk::CommandPool command_pool;

  ::vk::Extent2D output_size;

  VmaAllocator allocator;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;

  std::map<std::string, Window> windows_;
  std::queue<std::string> stack;

  Inputs io;
  Inputs previous_io;
};

}  // namespace wren::gui
