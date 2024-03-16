#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <span>
#include <vulkan/vulkan.hpp>
#include <wrenm/vector.hpp>

#include "buffer.hpp"
#include "utils/device.hpp"

namespace wren {

struct Vertex {
  wrenm::vec2 pos;
  wrenm::vec3 colour;
};

std::vector<Vertex> const TRIANGLE_VERTICES = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

class Mesh {
 public:
  Mesh() = default;

  Mesh(vulkan::Device const& device, VmaAllocator allocator)
      : vertices(TRIANGLE_VERTICES) {
    std::span data{vertices.begin(), vertices.end()};

    auto staging_buffer =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT);

    staging_buffer->set_data_raw<Vertex>(allocator, data);

    vertex_buffer_ =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                           VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    Buffer::copy_buffer(device, device.command_pool(), staging_buffer,
                        vertex_buffer_, data.size_bytes());
  }

  void draw(vk::CommandBuffer const& cmd);
  void bind(vk::CommandBuffer const& cmd);

 private:
  std::vector<Vertex> vertices;
  std::shared_ptr<Buffer> vertex_buffer_;
};

}  // namespace wren
