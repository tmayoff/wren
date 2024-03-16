#pragma once

#include <vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>
#include <wrenm/vector.hpp>

#include "buffer.hpp"

namespace wren {

struct Vertex {
  wrenm::vec2 pos;
  wrenm::vec3 colour;
};

class Mesh {
 public:
  Mesh() = default;

  Mesh(VmaAllocator allocator) {
    const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    const size_t size = sizeof(Vertex) * vertices.size();
    void* buf = std::malloc(size);
    std::memcpy(buf, vertices.data(), size);

    vertex_buffer_ = Buffer::Create(
        allocator, size, vk::BufferUsageFlagBits::eVertexBuffer);

    vertex_buffer_->set_data_raw(allocator, size, buf);
  }

  void bind(const vk::CommandBuffer& cmd);

 private:
  std::vector<Vertex> vertices;
  std::shared_ptr<Buffer> vertex_buffer_;
};

}  // namespace wren
