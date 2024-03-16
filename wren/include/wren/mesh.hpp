#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <span>
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
    vertices = {{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

    std::span data{vertices.begin(), vertices.end()};

    vertex_buffer_ =
        Buffer::Create(allocator, data.size_bytes(),
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    vertex_buffer_->set_data_raw<Vertex>(allocator, data);
  }

  void draw(vk::CommandBuffer const& cmd);
  void bind(vk::CommandBuffer const& cmd);

 private:
  std::vector<Vertex> vertices;
  std::shared_ptr<Buffer> vertex_buffer_;
};

}  // namespace wren
