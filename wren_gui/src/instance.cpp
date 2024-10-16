#include "instance.hpp"

#include <spdlog/spdlog.h>

#include <wren/vk/buffer.hpp>
#include <wren/math/geometry.hpp>
#include <wren/math/vector.hpp>
#include <wren_text/font.hpp>

namespace wren::gui {

Instance::Instance(const std::shared_ptr<vk::Shader>& shader,
                   const ::vk::Device& device, const VmaAllocator& allocator,
                   const ::vk::CommandPool& command_pool,
                   const ::vk::Queue& graphics_queue)
    : shader_(shader),
      device(device),
      graphics_queue(graphics_queue),
      command_pool(command_pool),
      allocator(allocator) {
  // ================ Vertex buffer =================== //
  {
    vertex_buffer = vk::Buffer::create(
        allocator, sizeof(Vertex) * MAX_VERTICES,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  // ============== Index buffer ============== //
  {
    index_buffer = vk::Buffer::create(
        allocator, sizeof(uint16_t) * MAX_INDICES,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  {
    const UBO ubo{};
    const std::size_t size = sizeof(ubo);

    uniform_buffer = vk::Buffer::create(
        allocator, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VmaAllocationCreateFlagBits::
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    uniform_buffer->set_data_raw(&ubo, size);
  }

  text::load_default_font();
}

void Instance::Begin() {
  // Mouse down
  io.left_mouse_down = io.left_mouse && !previous_io.left_mouse;
}

void Instance::End() { previous_io = io; }

void Instance::flush(const ::vk::CommandBuffer& cmd) {
  if (vertices.empty() || indices.empty()) return;

  {
    std::span data{vertices.begin(), vertices.end()};

    auto staging_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<Vertex>(data);

    vk::Buffer::copy_buffer(device, graphics_queue, command_pool,
                            staging_buffer, vertex_buffer, data.size_bytes());
  }

  {
    std::span data{indices.begin(), indices.end()};

    auto staging_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<uint16_t>(data);

    vk::Buffer::copy_buffer(device, graphics_queue, command_pool,
                            staging_buffer, index_buffer, data.size_bytes());
  }

  UBO ubo{};
  ubo.proj = wren::math::ortho(0.0f, static_cast<float>(output_size.width),
                               0.0f, static_cast<float>(output_size.height));
  uniform_buffer->set_data_raw(&ubo, sizeof(UBO));

  ::vk::DescriptorBufferInfo buffer_info{uniform_buffer->get(), 0, sizeof(UBO)};
  std::array writes{::vk::WriteDescriptorSet{
      {}, 0, 0, ::vk::DescriptorType::eUniformBuffer, {}, buffer_info}};

  cmd.pushDescriptorSetKHR(::vk::PipelineBindPoint::eGraphics,
                           shader_->pipeline_layout(), 0, writes);

  cmd.bindIndexBuffer(index_buffer->get(), 0, ::vk::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), {0});
  cmd.drawIndexed(indices.size(), 1, 0, 0, 0);

  vertices.clear();
  indices.clear();
}

auto Instance::BeginWindow(const std::string& name,
                           const wren::math::vec2f& size) -> bool {
  if (!windows_.contains(name)) {
    const wren::math::vec2f pos = {
        static_cast<float>(output_size.width) / 2.0f,
        static_cast<float>(output_size.height) / 2.0f};
    const bool hovered = point_in_bounds(io.mouse_position, pos, size);
    windows_.emplace(name, Window{name, pos, size, hovered});
  }

  stack.push(name);

  auto& window = windows_.at(name);
  const bool hovered =
      point_in_bounds(io.mouse_position, windows_.at(name).pos, size);
  //  spdlog::debug("Window: ({}, {}), hovered: {}", window.pos.x(),
  //              window.pos.y(), hovered);

  if (io.left_mouse_down && hovered) {
    window.selected = true;
    window.mouse_offset = window.pos - io.mouse_position;
  }
  if (!io.left_mouse) {
    window.selected = false;
  }

  return hovered;
}

void Instance::EndWindow() {
  const auto& window_name = stack.front();
  stack.pop();

  if (windows_.contains(window_name)) {
    auto& window = windows_.at(window_name);
    draw_quad(window.pos, window.size,
              window.hovered ? wren::math::Vec4f{1.0, 1.0, 1.0, 1.0}
                             : wren::math::Vec4f{0.5f, 0.5f, 0.5f, 1.0f});

    if (window.selected) {
      // Move window
      window.pos = io.mouse_position + window.mouse_offset;
    }
  }
}

void Instance::draw_quad(const wren::math::vec2f& pos,
                         const wren::math::vec2f& size,
                         const wren::math::Vec4f& colour) {
  std::array quad_vertices = {
      Vertex{(wren::math::vec2f{0.0, 0.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{1.0, 0.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{1.0, 1.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{0.0, 1.0} * size) + pos, colour},
  };
  std::array quad_indices = {
      0, 1, 2, 2, 3, 0,
  };

  vertices.insert(vertices.end(), quad_vertices.begin(), quad_vertices.end());

  indices.insert(indices.end(), quad_indices.begin(), quad_indices.end());
}

auto Instance::point_in_bounds(const wren::math::vec2f& p,
                               const wren::math::vec2f& pos,
                               const wren::math::vec2f& size) -> bool {
  const wren::math::vec2f top_left = pos;
  const wren::math::vec2f bottom_right = pos + size;

  return top_left.x() <= p.x() && p.x() <= bottom_right.x() &&
         top_left.y() <= p.y() && p.y() <= bottom_right.y();
}

}  // namespace wren::gui
