#include "instance.hpp"

#include <wren_vk/buffer.hpp>
#include <wrenm/geometry.hpp>

namespace wren::gui {

Instance::Instance(std::shared_ptr<vk::Shader> const& shader,
                   VK_NS::Device const& device,
                   VmaAllocator const& allocator,
                   VK_NS::CommandPool const& command_pool,
                   VK_NS::Queue const& graphics_queue)
    : shader_(shader),
      device(device),
      allocator(allocator),
      graphics_queue(graphics_queue),
      command_pool(command_pool) {
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
}

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

  float const aspect_ratio = static_cast<float>(size.width) /
                             static_cast<float>(size.height);
  UBO ubo{};
  ubo.proj = wrenm::ortho(-aspect_ratio, aspect_ratio, -1.0f, 1.0f);
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

auto Instance::BeginWindow() -> bool {
  draw_quad();
  return true;
}

auto Instance::EndWindow() -> bool { return true; }

void Instance::draw_quad() {
  static std::array quad_vertices = {
      Vertex{{-0.5, -0.5}, {1.0, 1.0, 1.0, 1.0}},
      Vertex{{0.5, -0.5}, {1.0, 1.0, 1.0, 1.0}},
      Vertex{{0.5, 0.5}, {1.0, 1.0, 1.0, 1.0}},
      Vertex{{-0.5, 0.5}, {1.0, 1.0, 1.0, 1.0}},
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
