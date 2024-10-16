#pragma once

#include <queue>
#include <vulkan/vulkan.hpp>
#include <wren/vk/buffer.hpp>
#include <wren/vk/shader.hpp>
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

static const std::size_t MAX_VERTICES = 10000;
static const std::size_t MAX_INDICES = MAX_VERTICES * 10;

struct Vertex {
  wren::math::vec2f pos;
  wren::math::Vec4f colour;
};

struct UBO {
  wren::math::Mat4f proj;
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
  Instance(const std::shared_ptr<vk::Shader>& shader,
           const ::vk::Device& device, const VmaAllocator& allocator,
           const ::vk::CommandPool& command_pool,
           const ::vk::Queue& graphics_queue);

  [[nodiscard]] auto shader() const { return shader_; }

  void bind(const ::vk::CommandBuffer& cmd);
  void flush(const ::vk::CommandBuffer& cmd);

  void resize_viewport(const ::vk::Extent2D& size) { this->output_size = size; }

  void Begin();
  void End();

  auto BeginWindow(const std::string& name, const wren::math::vec2f& size)
      -> bool;
  void EndWindow();

  auto IO() -> Inputs& { return io; }

 private:
  void draw_quad(const wren::math::vec2f& pos, const wren::math::vec2f& size,
                 const wren::math::Vec4f& colour);

  auto point_in_bounds(const wren::math::vec2f& p, const wren::math::vec2f& pos,
                       const wren::math::vec2f& size) -> bool;

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
