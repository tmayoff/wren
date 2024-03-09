#include "wren/shader.hpp"

#include <shaderc/shaderc.h>
#include <shaderc/status.h>
#include <vulkan/vulkan_core.h>
#include <wren_reflect/spirv_reflect.h>

#include <shaderc/shaderc.hpp>
#include <system_error>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_reflect/parser.hpp>
#include <wrenm/vector.hpp>

#include "tl/expected.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include "wren/window.hpp"

namespace wren {

ShaderModule::ShaderModule(reflect::spirv_t spirv,
                           const vk::ShaderModule &module)
    : spirv(std::move(spirv)),
      module(module),
      reflection(
          std::make_shared<spv_reflect::ShaderModule>(this->spirv)) {}

auto ShaderModule::get_vertex_input_bindings() const
    -> std::vector<vk::VertexInputBindingDescription> {
  uint32_t count = 0;
  reflection->EnumerateInputVariables(&count, nullptr);
  std::vector<SpvReflectInterfaceVariable *> input_variables(count);
  reflection->EnumerateInputVariables(&count, input_variables.data());

  uint32_t offset = 0;
  for (const auto &input : input_variables) {
    const auto width = input->numeric.scalar.width / 8;
    const auto count = input->numeric.vector.component_count;
    offset += width * count;
  }

  return {{0, offset}};
}

auto ShaderModule::get_vertex_input_attributes() const
    -> std::vector<vk::VertexInputAttributeDescription> {
  uint32_t count = 0;
  reflection->EnumerateInputVariables(&count, nullptr);
  std::vector<SpvReflectInterfaceVariable *> input_variables(count);
  reflection->EnumerateInputVariables(&count, input_variables.data());

  uint32_t offset = 0;
  std::vector<vk::VertexInputAttributeDescription> attrs;
  for (const auto &input : input_variables) {
    attrs.emplace_back(input->location, 0,
                       static_cast<vk::Format>(input->format),
                       offset);
    offset += (input->numeric.scalar.width / 8) *
              input->numeric.vector.component_count;
  }

  return attrs;
}

auto Shader::Create(const vulkan::Device &device,
                    const std::string &vertex_shader,
                    const std::string &fragment_shader)
    -> tl::expected<std::shared_ptr<Shader>, std::error_code> {
  auto shader = std::make_shared<Shader>();

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

  shader->vertex_shader(v_result.value());
  shader->fragment_shader(f_result.value());

  return shader;
}

auto Shader::compile_shader(const vk::Device &device,
                            const shaderc_shader_kind &shader_kind,
                            const std::string &filename,
                            const std::string &shader_source)
    -> tl::expected<ShaderModule, std::error_code> {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  const auto compilation_result = compiler.CompileGlslToSpv(
      shader_source, shader_kind, filename.c_str());

  if (compilation_result.GetCompilationStatus() !=
      shaderc_compilation_status_success) {
    return tl::make_unexpected(
        std::make_error_code(std::errc::invalid_argument));
  }

  std::span spirv(compilation_result.cbegin(),
                  compilation_result.cend());
  vk::ShaderModuleCreateInfo create_info({}, spirv);

  auto [res, module] = device.createShaderModule(create_info);
  if (res != vk::Result::eSuccess) {
    return tl::make_unexpected(make_error_code(res));
  }

  return ShaderModule{{spirv.begin(), spirv.end()}, module};
}

auto Shader::create_graphics_pipeline(
    const vk::Device &device, const vk::RenderPass &render_pass,
    const vk::Extent2D &size) -> tl::expected<void, std::error_code> {
  vk::Result res = vk::Result::eSuccess;

  vk::PipelineLayoutCreateInfo layout_create;
  std::tie(res, pipeline_layout) =
      device.createPipelineLayout(layout_create);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::array dynamic_states = {vk::DynamicState::eViewport,
                               vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dynamic_state({},
                                                   dynamic_states);

  const auto input_bindings =
      vertex_shader_module.get_vertex_input_bindings();
  const auto input_attributes =
      vertex_shader_module.get_vertex_input_attributes();

  vk::PipelineVertexInputStateCreateInfo vertex_input_info{
      {}, input_bindings, input_attributes};

  vk::PipelineInputAssemblyStateCreateInfo input_assembly(
      {}, vk::PrimitiveTopology::eTriangleList, false);

  vk::Viewport viewport{0,
                        0,
                        static_cast<float>(size.width),
                        static_cast<float>(size.height),
                        0,
                        1};
  vk::Rect2D scissor{{0, 0}, size};
  vk::PipelineViewportStateCreateInfo viewport_state{
      {}, viewport, scissor};

  vk::PipelineRasterizationStateCreateInfo rasterization(
      {}, false, false, vk::PolygonMode::eFill,
      vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, false,
      {}, {}, {}, 1.0f);

  vk::PipelineMultisampleStateCreateInfo multisample{
      {}, vk::SampleCountFlagBits::e1, false};

  vk::PipelineColorBlendAttachmentState colour_blend_attachment{
      false,
      vk::BlendFactor::eSrcAlpha,
      vk::BlendFactor::eOneMinusSrcAlpha,
      vk::BlendOp::eAdd,
      vk::BlendFactor::eOne,
      vk::BlendFactor::eZero,
      vk::BlendOp::eAdd};
  colour_blend_attachment.setColorWriteMask(
      vk::ColorComponentFlagBits::eR |
      vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB |
      vk::ColorComponentFlagBits::eA);
  vk::PipelineColorBlendStateCreateInfo colour_blend(
      {}, false, vk::LogicOp::eCopy, colour_blend_attachment,
      {0.0, 0.0, 0.0, 0.0});

  auto v_stage_create_info = vk::PipelineShaderStageCreateInfo(
      {}, vk::ShaderStageFlagBits::eVertex,
      vertex_shader_module.module, "main");
  auto f_stage_create_info = vk::PipelineShaderStageCreateInfo(
      {}, vk::ShaderStageFlagBits::eFragment,
      fragment_shader_module.module, "main");
  std::array shader_stages = {v_stage_create_info,
                              f_stage_create_info};

  auto create_info = vk::GraphicsPipelineCreateInfo(
      {}, shader_stages, &vertex_input_info, &input_assembly, {},
      &viewport_state, &rasterization, &multisample, {},
      &colour_blend, &dynamic_state, pipeline_layout, render_pass);

  std::tie(res, pipeline) =
      device.createGraphicsPipeline({}, create_info);
  if (res != vk::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  return {};
}

}  // namespace wren
