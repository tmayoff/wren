#pragma once

#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wrenm/vector.hpp>

namespace wren::gui {

static std::size_t const MAX_VERTICES = 10000;
static std::size_t const MAX_INDICES = MAX_VERTICES * 10;

struct Vertex {
  wrenm::vec2f pos;
  wrenm::vec4f colour;
};

class Instance {
 public:
  Instance(VK_NS::Device const& device, VmaAllocator const& allocator,
           VK_NS::CommandPool const& command_pool,
           VK_NS::Queue const& graphics_queue);

  void bind(VK_NS::CommandBuffer const& cmd);
  void flush(VK_NS::CommandBuffer const& cmd);

  auto BeginWindow() -> bool;
  auto EndWindow() -> bool;

 private:
  void draw_quad();

  std::shared_ptr<wren::vk::Buffer> index_buffer;
  std::shared_ptr<wren::vk::Buffer> vertex_buffer;

  VK_NS::Device device;
  VK_NS::Queue graphics_queue;
  VK_NS::CommandPool command_pool;

  VmaAllocator allocator;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
};

}  // namespace wren::gui
