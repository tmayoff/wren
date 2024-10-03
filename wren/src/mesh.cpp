#include "mesh.hpp"

#include <chrono>
#include <vulkan/vulkan.hpp>
#include <wren_math/geometry.hpp>
#include <wren_math/vector.hpp>

namespace wren {

Mesh::Mesh(const vulkan::Device& device, VmaAllocator allocator)
    : vertices(kQuadVertices), indices(kQuadIndices) {
  // ================ Vertex buffer =================== //
  {
    std::span data{vertices.begin(), vertices.end()};
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
    std::span data{indices.begin(), indices.end()};

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
  cmd.drawIndexed(indices.size(), 1, 0, 0, 0);
}

void Mesh::bind(const ::vk::CommandBuffer& cmd) const {
  static auto start_time = std::chrono::high_resolution_clock::now();
  auto current_time = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(
                   current_time - start_time)
                   .count();

  UBO ubo{};
  ubo.model =
      wren::math::rotate(wren::math::mat4f{}, time * wren::math::radians(90.0f),
                         wren::math::vec3f(0.0f, 0.0f, 1.0f));

  uniform_buffer->set_data_raw(&ubo, sizeof(ubo));

  ::vk::DescriptorBufferInfo buffer_info(uniform_buffer->get(), 0, sizeof(UBO));
  std::array writes = {::vk::WriteDescriptorSet{
      {}, 0, 0, ::vk::DescriptorType::eUniformBuffer, {}, buffer_info}};

  cmd.pushDescriptorSetKHR(::vk::PipelineBindPoint::eGraphics,
                           shader_->pipeline_layout(), 1, writes);

  cmd.bindIndexBuffer(index_buffer->get(), 0, ::vk::IndexType::eUint16);
  cmd.bindVertexBuffers(0, vertex_buffer->get(), ::vk::DeviceSize{0});
}

}  // namespace wren
