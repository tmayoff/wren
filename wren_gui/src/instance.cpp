#include "instance.hpp"

#include <spdlog/spdlog.h>

#include <wren_text/font.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_math/geometry.hpp>
#include <wren_math/vector.hpp>

namespace wren::gui {

Instance::Instance(std::shared_ptr<vk::Shader> const& shader,
                   VK_NS::Device const& device,
                   VmaAllocator const& allocator,
                   VK_NS::CommandPool const& command_pool,
                   VK_NS::Queue const& graphics_queue)
    : shader_(shader),
      device(device),
      graphics_queue(graphics_queue),
      command_pool(command_pool),
      allocator(allocator) {
  // ================ Vertex buffer =================== //
  {
    vertex_buffer =
        vk::Buffer::Create(allocator, sizeof(Vertex) * MAX_VERTICES,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  // ============== Index buffer ============== //
  {
    index_buffer =
        vk::Buffer::Create(allocator, sizeof(uint16_t) * MAX_INDICES,
                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  }

  {
    const UBO ubo{};
    std::size_t const size = sizeof(ubo);

    uniform_buffer = vk::Buffer::Create(
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

void Instance::flush(VK_NS::CommandBuffer const& cmd) {
  if (vertices.empty() || indices.empty()) return;

  {
    std::span data{vertices.begin(), vertices.end()};

    auto staging_buffer = vk::Buffer::Create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<Vertex>(data);

    vk::Buffer::copy_buffer(device, graphics_queue, command_pool,
                            staging_buffer, vertex_buffer,
                            data.size_bytes());
  }

  {
    std::span data{indices.begin(), indices.end()};

    auto staging_buffer = vk::Buffer::Create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<uint16_t>(data);

    vk::Buffer::copy_buffer(device, graphics_queue, command_pool,
                            staging_buffer, index_buffer,
                            data.size_bytes());
  }

  float const aspect_ratio = static_cast<float>(output_size.width) /
                             static_cast<float>(output_size.height);
  UBO ubo{};
  ubo.proj =
      wren::math::ortho(0.0f, static_cast<float>(output_size.width), 0.0f,
                   static_cast<float>(output_size.height));
  uniform_buffer->set_data_raw(&ubo, sizeof(UBO));

  VK_NS::DescriptorBufferInfo buffer_info{uniform_buffer->get(), 0,
                                          sizeof(UBO)};
  std::array writes{
      VK_NS::WriteDescriptorSet{{},
                                0,
                                0,
                                VK_NS::DescriptorType::eUniformBuffer,
                                {},
                                buffer_info}};

  cmd.pushDescriptorSetKHR(VK_NS::PipelineBindPoint::eGraphics,
                           shader_->pipeline_layout(), 0, writes);

  cmd.bindIndexBuffer(index_buffer->get(), 0,
                      VK_NS::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), {0});
  cmd.drawIndexed(indices.size(), 1, 0, 0, 0);

  vertices.clear();
  indices.clear();
}

auto Instance::BeginWindow(std::string const& name,
                           wren::math::vec2f const& size) -> bool {
  if (!windows_.contains(name)) {
    wren::math::vec2f const pos = {
        static_cast<float>(output_size.width) / 2.0f,
        static_cast<float>(output_size.height) / 2.0f};
    bool const hovered =
        point_in_bounds(io.mouse_position, pos, size);
    windows_.emplace(name, Window{name, pos, size, hovered});
  }

  stack.push(name);

  auto& window = windows_.at(name);
  bool const hovered =
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
  auto const& window_name = stack.front();
  stack.pop();

  if (windows_.contains(window_name)) {
    auto& window = windows_.at(window_name);
    draw_quad(window.pos, window.size,
              window.hovered ? wren::math::vec4f{1.0, 1.0, 1.0, 1.0}
                             : wren::math::vec4f{0.5f, 0.5f, 0.5f, 1.0f});

    if (window.selected) {
      // Move window
      window.pos = io.mouse_position + window.mouse_offset;
    }
  }
}

void Instance::draw_quad(wren::math::vec2f const& pos,
                         wren::math::vec2f const& size,
                         wren::math::vec4f const& colour) {
  std::array quad_vertices = {
      Vertex{(wren::math::vec2f{0.0, 0.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{1.0, 0.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{1.0, 1.0} * size) + pos, colour},
      Vertex{(wren::math::vec2f{0.0, 1.0} * size) + pos, colour},
  };
  std::array quad_indices = {
      0, 1, 2, 2, 3, 0,
  };

  vertices.insert(vertices.end(), quad_vertices.begin(),
                  quad_vertices.end());

  indices.insert(indices.end(), quad_indices.begin(),
                 quad_indices.end());
}

auto Instance::point_in_bounds(wren::math::vec2f const& p,
                               wren::math::vec2f const& pos,
                               wren::math::vec2f const& size) -> bool {
  wren::math::vec2f const top_left = pos;
  wren::math::vec2f const bottom_right = pos + size;

  return top_left.x() <= p.x() && p.x() <= bottom_right.x() &&
         top_left.y() <= p.y() && p.y() <= bottom_right.y();
}

}  // namespace wren::gui
