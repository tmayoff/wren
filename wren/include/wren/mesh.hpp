#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <wrenm/vector.hpp>

#include "buffer.hpp"
#include "utils/device.hpp"

namespace wren {

struct Vertex {
  wrenm::vec2f pos;
  wrenm::vec3f colour;
};

std::vector<Vertex> const TRIANGLE_VERTICES = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};

std::vector<Vertex> const QUAD_VERTICES = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}};

std::vector<uint16_t> const QUAD_INDICES = {0, 1, 2, 2, 3, 0};

class Mesh {
 public:
  Mesh() = default;

  Mesh(vulkan::Device const& device, VmaAllocator allocator);

  void draw(vk::CommandBuffer const& cmd);
  void bind(vk::CommandBuffer const& cmd);

 private:
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  std::shared_ptr<Buffer> index_buffer;
  std::shared_ptr<Buffer> vertex_buffer;
};

}  // namespace wren
