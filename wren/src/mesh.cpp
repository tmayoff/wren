#include "mesh.hpp"

#include <vulkan/vulkan.hpp>

namespace wren {

Mesh::Mesh(vulkan::Device const& device, VmaAllocator allocator)
    : vertices(QUAD_VERTICES), indices(QUAD_INDICES) {
  // ================ Vertex buffer =================== //
  {
    std::span data{vertices.begin(), vertices.end()};
    auto staging_buffer =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<Vertex>(allocator, data);

    vertex_buffer =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    Buffer::copy_buffer(device, device.command_pool(), staging_buffer,
                        vertex_buffer, data.size_bytes());
  }

  // ============== Index buffer ============== //
  {
    std::span data{indices.begin(), indices.end()};

    auto staging_buffer =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<uint16_t>(allocator, data);

    index_buffer =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    Buffer::copy_buffer(device, device.command_pool(), staging_buffer,
                        index_buffer, data.size_bytes());
  }
}

void Mesh::draw(vk::CommandBuffer const& cmd) {
  cmd.drawIndexed(indices.size(), 1, 0, 0, 0);
}

void Mesh::bind(vk::CommandBuffer const& cmd) {
  cmd.bindIndexBuffer(index_buffer->get(), 0, vk::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), vk::DeviceSize{0});
}

}  // namespace wren
