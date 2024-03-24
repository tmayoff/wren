#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

#include <vulkan/vulkan.hpp>
#include <wren_vk/buffer.hpp>
#include <wren_vk/shader.hpp>
#include <wrenm/matrix.hpp>
#include <wrenm/vector.hpp>

#include "utils/device.hpp"

namespace wren {

struct UBO {
  wrenm::mat4f model;
  wrenm::mat4f view;
  wrenm::mat4f proj;
};

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

  void shader(std::shared_ptr<vk::Shader> const& shader_) {
    this->shader_ = shader_;
  }
  void draw(VK_NS::CommandBuffer const& cmd) const;
  void bind(VK_NS::CommandBuffer const& cmd) const;

 private:
  std::shared_ptr<vk::Shader> shader_;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  std::shared_ptr<vk::Buffer> index_buffer;
  std::shared_ptr<vk::Buffer> vertex_buffer;
  std::shared_ptr<vk::Buffer> uniform_buffer;
};

}  // namespace wren
