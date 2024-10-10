#include "mesh.hpp"

#include <vulkan/vulkan.hpp>
#include <wren_math/geometry.hpp>
#include <wren_math/vector.hpp>

namespace wren {

Mesh::Mesh(const vulkan::Device& device, VmaAllocator allocator)
    : vertices_(kQuadVertices.begin(), kQuadVertices.end()),
      indices_(kQuadIndices) {
  // ================ Vertex buffer =================== //
  {
    std::span data{vertices_.begin(), vertices_.end()};
    auto staging_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<Vertex>(data);

    vertex_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    vk::Buffer::copy_buffer(device.get(), device.get_graphics_queue(),
                            device.command_pool(), staging_buffer,
                            vertex_buffer, data.size_bytes());
  }

  // ============== Index buffer ============== //
  {
    std::span data{indices_.begin(), indices_.end()};

    auto staging_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<uint16_t>(data);

    index_buffer = vk::Buffer::create(
        allocator, data.size_bytes(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    vk::Buffer::copy_buffer(device.get(), device.get_graphics_queue(),
                            device.command_pool(), staging_buffer, index_buffer,
                            data.size_bytes());
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
}

void Mesh::draw(const ::vk::CommandBuffer& cmd) const {
  cmd.drawIndexed(indices_.size(), 1, 0, 0, 0);
}

void Mesh::bind(const ::vk::CommandBuffer& cmd) const {
  cmd.bindIndexBuffer(index_buffer->get(), 0, ::vk::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), ::vk::DeviceSize{0});
}

}  // namespace wren
