#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <wren_math/matrix.hpp>
#include <wren_math/vector.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/shader.hpp>

#include "utils/device.hpp"

namespace wren {

struct UBO {
  wren::math::Mat4f model;
};

struct Vertex {
  wren::math::Vec3f pos;
  wren::math::Vec3f colour;
};

const std::array kTriangleVertices = {
    Vertex{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    Vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    Vertex{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
};

const std::array kQuadVertices = {
    Vertex{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
    Vertex{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    Vertex{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    Vertex{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}};

const std::vector<uint16_t> kQuadIndices = {0, 1, 2, 2, 3, 0};

class Mesh {
 public:
  Mesh() = default;

  Mesh(const vulkan::Device& device, VmaAllocator allocator);

  void shader(const std::shared_ptr<vk::Shader>& shader) { shader_ = shader; }
  void draw(const ::vk::CommandBuffer& cmd) const;
  void bind(const ::vk::CommandBuffer& cmd) const;

 private:
  std::shared_ptr<vk::Shader> shader_;
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
  std::shared_ptr<vk::Buffer> index_buffer;
  std::shared_ptr<vk::Buffer> vertex_buffer;
  std::shared_ptr<vk::Buffer> uniform_buffer;
};

}  // namespace wren
