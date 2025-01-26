#pragma once

#include <filesystem>
#include <optional>
#include <wren/math/matrix.hpp>
#include <wren/mesh.hpp>
#include <wren/mesh_loader.hpp>

#include "wren/context.hpp"

namespace wren::scene::components {

class MeshRenderer {
 public:
  auto bind(const std::shared_ptr<Context>& ctx,
            const std::shared_ptr<vk::Shader>& shader,
            const ::vk::CommandBuffer& cmd, const math::Mat4f& model_mat) {
    if (!mesh_.has_value()) return;
    if (!mesh_->loaded())
      mesh_->load(ctx->graphics_context->Device(),
                  ctx->graphics_context->allocator());

    struct LOCALS {
      wren::math::Mat4f model;
    };
    LOCALS ubo{};

    ubo.model = model_mat;

    // Load UBO if not there
    if (ubo_ == nullptr) {
      ubo_ = vk::Buffer::create(
          ctx->graphics_context->allocator(), sizeof(ubo),
          VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
          VmaAllocationCreateFlagBits::
              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    }

    ubo_->set_data_raw(&ubo, sizeof(LOCALS));

    ::vk::DescriptorBufferInfo buffer_info(ubo_->get(), 0, sizeof(LOCALS));

    shader->desciptor_sets().at(1);
    // std::array writes = {::vk::WriteDescriptorSet{
    //     {}, 0, 0, ::vk::DescriptorType::eUniformBuffer, {}, buffer_info}};

    // cmd.pushDescriptorSetKHR(::vk::PipelineBindPoint::eGraphics,
    //                          shader->pipeline_layout(), 1, writes);
    // TODO get descriptor set here and bind

    mesh_->bind(cmd);
    mesh_->draw(cmd);
  }

  auto update_mesh(const std::filesystem::path& project_root,
                   const std::filesystem::path& mesh_path) -> expected<void> {
    mesh_file_ = mesh_path;

    TRY_RESULT(mesh_, load_mesh(project_root / mesh_path));

    return {};
  }

  [[nodiscard]] auto mesh() const { return mesh_; }
  [[nodiscard]] auto mesh_file() const { return mesh_file_; }

 private:
  std::optional<Mesh> mesh_;
  std::filesystem::path mesh_file_;
  std::shared_ptr<vk::Buffer> ubo_;
};

}  // namespace wren::scene::components
