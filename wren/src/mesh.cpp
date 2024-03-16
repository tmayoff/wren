#include "mesh.hpp"

#include <vulkan/vulkan.hpp>

namespace wren {

void Mesh::draw(vk::CommandBuffer const& cmd) {
  cmd.draw(vertices.size(), 1, 0, 0);
}

void Mesh::bind(vk::CommandBuffer const& cmd) {
  cmd.bindVertexBuffers(0, vertex_buffer_->get(), vk::DeviceSize{0});
}

}  // namespace wren
