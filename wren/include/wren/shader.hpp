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
               const vk::ShaderModule &module);

  [[nodiscard]] auto get_vertex_input_bindings() const
      -> std::vector<vk::VertexInputBindingDescription>;

  [[nodiscard]] auto get_vertex_input_attributes() const
      -> std::vector<vk::VertexInputAttributeDescription>;
};

class Shader {
 public:
  static auto Create(const vulkan::Device &device,
                     const std::string &vertex_shader,
                     const std::string &fragment_shader)
      -> tl::expected<std::shared_ptr<Shader>, std::error_code>;

  static auto compile_shader(const vk::Device &device,
                             const shaderc_shader_kind &shader_kind,
                             const std::string &filename,
                             const std::string &shader_source)
      -> tl::expected<ShaderModule, std::error_code>;

  [[nodiscard]] auto get_pipeline() const { return pipeline; }

  void fragment_shader(const ShaderModule &fragment) {
    fragment_shader_module = fragment;
  }

  void vertex_shader(const ShaderModule &vertex) {
    vertex_shader_module = vertex;
  }

  auto create_graphics_pipeline(const vk::Device &device,
                                const vk::RenderPass &render_pass,
                                const vk::Extent2D &size)
      -> tl::expected<void, std::error_code>;

 private:
  vk::PipelineLayout pipeline_layout;
  vk::Pipeline pipeline;

  ShaderModule vertex_shader_module;
  ShaderModule fragment_shader_module;
};

}  // namespace wren
