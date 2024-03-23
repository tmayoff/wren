#include "instance.hpp"

#include "wren_vk/buffer.hpp"

namespace wren::gui {

Instance::Instance(VK_NS::Device const& device,
                   VmaAllocator const& allocator,
                   VK_NS::CommandPool const& command_pool,
                   VK_NS::Queue const& graphics_queue)
    : device(device),
      allocator(allocator),
      graphics_queue(graphics_queue),
      command_pool(command_pool) {
  // ================ Vertex buffer =================== //
  {
    vertex_buffer =
        vk::Buffer::Create(allocator, sizeof(Vertex) * MAX_VERTICES,
                           VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }

  // ============== Index buffer ============== //
  {
    index_buffer =
        vk::Buffer::Create(allocator, sizeof(uint16_t) * MAX_INDICES,
                           VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                               VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  }
}

void Instance::draw(VK_NS::CommandBuffer const& cmd) {
  if (vertices.empty() || indices.empty()) return;

  {
    std::span data{indices.begin(), indices.end()};

    auto staging_buffer = vk::Buffer::Create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<uint16_t>(data);

    vk::Buffer::copy_buffer(device, graphics_queue, command_pool,
                            staging_buffer, vertex_buffer,
                            data.size_bytes());
  }

  cmd.bindIndexBuffer(index_buffer->get(),
                      VK_NS::DeviceSize{indices.size()},
                      VK_NS::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), {0});
  cmd.drawIndexed(indices.size(), 1, 0, 0, 0);

  vertices.clear();
  indices.clear();
}

auto Instance::BeginWindow() -> bool {
  draw_quad();
  return true;
}

auto Instance::EndWindow() -> bool { return true; }

void Instance::draw_quad() {
  static std::array quad_vertices = {
      Vertex{{-0.5, -0.5}},
      Vertex{{0.5, -0.5}},
      Vertex{{0.5, 0.5}},
      Vertex{{-0.5, 0.5}},
  };
  std::array quad_indices = {
      0, 1, 2, 2, 3, 0,
  };

  vertices.insert(vertices.end(), quad_vertices.begin(),
                  quad_vertices.end());

  indices.insert(indices.end(), quad_indices.begin(),
                 quad_indices.end());
}

}  // namespace wren::gui
