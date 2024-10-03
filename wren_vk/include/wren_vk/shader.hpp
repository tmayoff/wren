#pragma once

#include <shaderc/shaderc.h>
#include <shaderc/status.h>
#include <wren_reflect/spirv_reflect.h>

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren_math/vector.hpp>
#include <wren_reflect/parser.hpp>
#include <wren_utils/errors.hpp>

DEFINE_ERROR_IMPL("shaderc", shaderc_compilation_status)
BOOST_DESCRIBE_ENUM(shaderc_compilation_status,
                    shaderc_compilation_status_invalid_stage,
                    shaderc_compilation_status_compilation_error,
                    shaderc_compilation_status_internal_error,
                    shaderc_compilation_status_null_result_object,
                    shaderc_compilation_status_validation_error,
                    shaderc_compilation_status_transformation_error,
                    shaderc_compilation_status_configuration_error)

namespace wren::vk {

struct ShaderModule {
  reflect::spirv_t spirv;
  ::vk::ShaderModule module;
  std::shared_ptr<spv_reflect::ShaderModule> reflection;

  ShaderModule() = default;
  ShaderModule(reflect::spirv_t spirv, const ::vk::ShaderModule &module);

  [[nodiscard]] auto get_vertex_input_bindings() const
      -> std::vector<::vk::VertexInputBindingDescription>;

  [[nodiscard]] auto get_vertex_input_attributes() const
      -> std::vector<::vk::VertexInputAttributeDescription>;

  [[nodiscard]] auto get_descriptor_set_layout_bindings() const
      -> std::vector<::vk::DescriptorSetLayoutBinding>;
};

class Shader {
 public:
  static auto create(const ::vk::Device &device,
                     const std::string &vertex_shader,
                     const std::string &fragment_shader)
      -> expected<std::shared_ptr<Shader>>;

  static auto compile_shader(const ::vk::Device &device,
                             const shaderc_shader_kind &shader_kind,
                             const std::string &filename,
                             const std::string &shader_source)
      -> wren::expected<ShaderModule>;

  [[nodiscard]] auto get_pipeline() const { return pipeline_; }
  [[nodiscard]] auto pipeline_layout() const { return pipeline_layout_; }
  [[nodiscard]] auto descriptor_layout() const { return descriptor_layout_; }

  void fragment_shader(const ShaderModule &fragment) {
    fragment_shader_module_ = fragment;
  }

  void vertex_shader(const ShaderModule &vertex) {
    vertex_shader_module_ = vertex;
  }

  auto create_graphics_pipeline(const ::vk::Device &device,
                                const ::vk::RenderPass &render_pass,
                                const math::vec2i &size) -> expected<void>;

 private:
  ::vk::DescriptorSetLayout descriptor_layout_;
  ::vk::PipelineLayout pipeline_layout_;
  ::vk::Pipeline pipeline_;

  ShaderModule vertex_shader_module_;
  ShaderModule fragment_shader_module_;
};

}  // namespace wren::vk
