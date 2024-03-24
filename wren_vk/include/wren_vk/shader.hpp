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

namespace wren::vk {

struct ShaderModule {
  reflect::spirv_t spirv;
  VK_NS::ShaderModule module;
  std::shared_ptr<spv_reflect::ShaderModule> reflection;

  ShaderModule() = default;
  ShaderModule(reflect::spirv_t spirv,
               VK_NS::ShaderModule const &module);

  [[nodiscard]] auto get_vertex_input_bindings() const
      -> std::vector<VK_NS::VertexInputBindingDescription>;

  [[nodiscard]] auto get_vertex_input_attributes() const
      -> std::vector<VK_NS::VertexInputAttributeDescription>;
};

class Shader {
 public:
  static auto Create(VK_NS::Device const &device,
                     std::string const &vertex_shader,
                     std::string const &fragment_shader)
      -> tl::expected<std::shared_ptr<Shader>, std::error_code>;

  static auto compile_shader(VK_NS::Device const &device,
                             shaderc_shader_kind const &shader_kind,
                             std::string const &filename,
                             std::string const &shader_source)
      -> tl::expected<ShaderModule, std::error_code>;

  [[nodiscard]] auto get_pipeline() const { return pipeline; }
  [[nodiscard]] auto pipeline_layout() const {
    return pipeline_layout_;
  }
  [[nodiscard]] auto descriptor_layout() const {
    return descriptor_layout_;
  }

  void fragment_shader(ShaderModule const &fragment) {
    fragment_shader_module = fragment;
  }

  void vertex_shader(ShaderModule const &vertex) {
    vertex_shader_module = vertex;
  }

  auto create_graphics_pipeline(VK_NS::Device const &device,
                                VK_NS::RenderPass const &render_pass,
                                VK_NS::Extent2D const &size)
      -> tl::expected<void, std::error_code>;

 private:
  VK_NS::DescriptorSetLayout descriptor_layout_;
  VK_NS::PipelineLayout pipeline_layout_;
  VK_NS::Pipeline pipeline;

  ShaderModule vertex_shader_module;
  ShaderModule fragment_shader_module;
};

}  // namespace wren::vk
