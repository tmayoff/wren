#include "mesh.hpp"

#include <vulkan/vulkan.hpp>

namespace wren {

void Mesh::bind(const vk::CommandBuffer& cmd) {
  std::array<vk::Buffer, 1> bufs = {vertex_buffer_->get()};
  std::array<vk::DeviceSize, 1> offsets = {0};
  cmd.bindVertexBuffers(0, bufs, offsets);
}

}  // namespace wren
