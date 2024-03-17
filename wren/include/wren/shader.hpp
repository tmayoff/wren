#pragma once

#include <shaderc/shaderc.h>
#include <wren_reflect/spirv_reflect.h>

#include <memory>
#include <string>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_reflect/parser.hpp>

#include "wren/utils/device.hpp"

namespace wren {

struct ShaderModule {
  reflect::spirv_t spirv;
  vk::ShaderModule module;
  std::shared_ptr<spv_reflect::ShaderModule> reflection;

  ShaderModule() = default;
  ShaderModule(reflect::spirv_t spirv,
               vk::ShaderModule const &module);

  [[nodiscard]] auto get_vertex_input_bindings() const
      -> std::vector<vk::VertexInputBindingDescription>;

  [[nodiscard]] auto get_vertex_input_attributes() const
      -> std::vector<vk::VertexInputAttributeDescription>;
};

class Shader {
 public:
  static auto Create(vulkan::Device const &device,
                     std::string const &vertex_shader,
                     std::string const &fragment_shader)
      -> tl::expected<std::shared_ptr<Shader>, std::error_code>;

  static auto compile_shader(vk::Device const &device,
                             shaderc_shader_kind const &shader_kind,
                             std::string const &filename,
                             std::string const &shader_source)
      -> tl::expected<ShaderModule, std::error_code>;

  [[nodiscard]] auto get_pipeline() const { return pipeline; }

  void fragment_shader(ShaderModule const &fragment) {
    fragment_shader_module = fragment;
  }

  void vertex_shader(ShaderModule const &vertex) {
    vertex_shader_module = vertex;
  }

  auto create_graphics_pipeline(vk::Device const &device,
                                vk::RenderPass const &render_pass,
                                vk::Extent2D const &size)
      -> tl::expected<void, std::error_code>;

 private:
  vk::DescriptorSetLayout descriptor_layout;
  vk::PipelineLayout pipeline_layout;
  vk::Pipeline pipeline;

  ShaderModule vertex_shader_module;
  ShaderModule fragment_shader_module;
};

}  // namespace wren
