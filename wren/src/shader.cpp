#include "wren/shader.hpp"
#include "tl/expected.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "wren/utils/spirv_cross.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include "wren/window.hpp"
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <system_error>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

ShaderModule::ShaderModule(spirv_t spirv, const vk::ShaderModule &module)
    : spirv(std::move(spirv)), module(module),
      glsl(std::make_shared<spirv::CompilerGLSL>(this->spirv)) {}

auto ShaderModule::get_shader_stage_info() const -> vk::PipelineShaderStageCreateInfo {
  const auto entry_points = glsl->get_entry_point_and_shader_stages();
  // const auto entry_point = entry_points.front();
  // const auto stage =
  //       wren::spirv::get_vk_shader_stage(entry_point.execution_model);

  // return {{}, stage.value(), module, entry_point.name.c_str()};

  return {};
}


auto ShaderModule::get_vertex_input() const -> vk::PipelineVertexInputStateCreateInfo {
  // const auto resources = glsl->get_shader_resources();

  // TODO Vertex binding descriptors
  std::vector<vk::VertexInputBindingDescription> binding_descriptions;

  // TODO Vertex attribute descriptors
  std::vector<vk::VertexInputAttributeDescription> attribute_descriptions;

  return {{}, binding_descriptions, attribute_descriptions};
}

auto Shader::Create(const vulkan::Device &device,
                    const std::string &vertex_shader,
                    const std::string &fragment_shader)
    -> tl::expected<Shader, std::error_code> {
  Shader shader;

  auto v_result = compile_shader(
      device.get(), shaderc_shader_kind::shaderc_glsl_vertex_shader,
      "vertex_shader", vertex_shader);
  if (!v_result.has_value()) {
    return tl::make_unexpected(v_result.error());
  }

  auto f_result = compile_shader(
      device.get(), shaderc_shader_kind::shaderc_glsl_fragment_shader,
      "fragment_shader", fragment_shader);
  if (!f_result.has_value()) {
    return tl::make_unexpected(f_result.error());
  }

  shader.vertex_shader(v_result.value());
  shader.fragment_shader(f_result.value());

  return shader;
}

auto Shader::compile_shader(const vk::Device &device,
                            const shaderc_shader_kind &shader_kind,
                            const std::string &filename,
                            const std::string &shader_source)
    -> tl::expected<ShaderModule, std::error_code> {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  const auto compilation_result =
      compiler.CompileGlslToSpv(shader_source, shader_kind, filename.c_str());

  if (compilation_result.GetCompilationStatus() !=
      shaderc_compilation_status_success) {
    return tl::make_unexpected(
        std::make_error_code(std::errc::invalid_argument));
  }

  std::span spirv(compilation_result.cbegin(), compilation_result.cend());
  vk::ShaderModuleCreateInfo create_info({}, spirv);

  auto [res, module] = device.createShaderModule(create_info);
  if (res != vk::Result::eSuccess) {
    return tl::make_unexpected(make_error_code(res));
  }

  return ShaderModule{{spirv.begin(), spirv.end()}, module};
}

auto Shader::reflect_pipeline_layout() -> vk::PipelineLayoutCreateInfo {
  const auto v_stage_create_info = vertex_shader_module.get_shader_stage_info();
  const auto f_stage_create_info =
      fragment_shader_module.get_shader_stage_info();

  std::array shader_stages = {v_stage_create_info, f_stage_create_info};

  std::array dynamic_states = {vk::DynamicState::eViewport,
                               vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_states);

  const auto vertex_input_info = vertex_shader_module.get_vertex_input();

  vk::PipelineInputAssemblyStateCreateInfo input_assembly(
      {}, vk::PrimitiveTopology::eTriangleList, false);
}

} // namespace wren
