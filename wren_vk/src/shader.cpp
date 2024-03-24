#include "wren_vk/shader.hpp"

#include <shaderc/shaderc.h>
#include <shaderc/status.h>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan_core.h>
#include <wren_reflect/spirv_reflect.h>

#include <shaderc/shaderc.hpp>
#include <system_error>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <wren_reflect/parser.hpp>
#include <wren_vk/errors.hpp>

#include "vulkan/vulkan_structs.hpp"

namespace wren::vk {

ShaderModule::ShaderModule(reflect::spirv_t spirv,
                           VK_NS::ShaderModule const &module)
    : spirv(std::move(spirv)),
      module(module),
      reflection(
          std::make_shared<spv_reflect::ShaderModule>(this->spirv)) {}

auto ShaderModule::get_vertex_input_bindings() const
    -> std::vector<VK_NS::VertexInputBindingDescription> {
  uint32_t count = 0;
  reflection->EnumerateInputVariables(&count, nullptr);
  std::vector<SpvReflectInterfaceVariable *> input_variables(count);
  reflection->EnumerateInputVariables(&count, input_variables.data());

  uint32_t offset = 0;
  for (auto const &input : input_variables) {
    auto const width = input->numeric.scalar.width / 8;
    auto const count = input->numeric.vector.component_count;
    offset += width * count;
  }

  return {{0, offset}};
}

auto ShaderModule::get_vertex_input_attributes() const
    -> std::vector<VK_NS::VertexInputAttributeDescription> {
  uint32_t count = 0;
  reflection->EnumerateInputVariables(&count, nullptr);
  std::vector<SpvReflectInterfaceVariable *> input_variables(count);
  reflection->EnumerateInputVariables(&count, input_variables.data());

  uint32_t offset = 0;
  std::vector<VK_NS::VertexInputAttributeDescription> attrs;
  for (auto const &input : input_variables) {
    if (input->location == UINT32_MAX) continue;
    attrs.emplace_back(input->location, 0,
                       static_cast<VK_NS::Format>(input->format),
                       offset);
    offset += (input->numeric.scalar.width / 8) *
              input->numeric.vector.component_count;
  }

  return attrs;
}

auto Shader::Create(VK_NS::Device const &device,
                    std::string const &vertex_shader,
                    std::string const &fragment_shader)
    -> expected<std::shared_ptr<Shader>> {
  auto shader = std::make_shared<Shader>();

  auto v_result = compile_shader(
      device, shaderc_shader_kind::shaderc_glsl_vertex_shader,
      "vertex_shader", vertex_shader);
  if (!v_result.has_value()) {
    return tl::make_unexpected(v_result.error());
  }

  auto f_result = compile_shader(
      device, shaderc_shader_kind::shaderc_glsl_fragment_shader,
      "fragment_shader", fragment_shader);
  if (!f_result.has_value()) {
    return tl::make_unexpected(f_result.error());
  }

  shader->vertex_shader(v_result.value());
  shader->fragment_shader(f_result.value());

  return shader;
}

auto Shader::compile_shader(VK_NS::Device const &device,
                            shaderc_shader_kind const &shader_kind,
                            std::string const &filename,
                            std::string const &shader_source)
    -> tl::expected<ShaderModule, std::error_code> {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  auto const compilation_result = compiler.CompileGlslToSpv(
      shader_source, shader_kind, filename.c_str());

  auto const compilation_status =
      compilation_result.GetCompilationStatus();
  if (compilation_status != shaderc_compilation_status_success) {
    spdlog::error("{}", compilation_result.GetErrorMessage());
    return tl::make_unexpected(make_error_code(compilation_status));
  }

  std::span spirv(compilation_result.cbegin(),
                  compilation_result.cend());
  VK_NS::ShaderModuleCreateInfo create_info({}, spirv);

  auto [res, module] = device.createShaderModule(create_info);
  if (res != VK_NS::Result::eSuccess) {
    return tl::make_unexpected(make_error_code(res));
  }

  return ShaderModule{{spirv.begin(), spirv.end()}, module};
}

auto Shader::create_graphics_pipeline(
    VK_NS::Device const &device, VK_NS::RenderPass const &render_pass,
    VK_NS::Extent2D const &size)
    -> tl::expected<void, std::error_code> {
  VK_NS::Result res = VK_NS::Result::eSuccess;

  VK_NS::DescriptorSetLayoutBinding ubo_layout_bindings(
      0, VK_NS::DescriptorType::eUniformBuffer, 1,
      VK_NS::ShaderStageFlagBits::eVertex);
  VK_NS::DescriptorSetLayoutCreateInfo dl_create_info(
      VK_NS::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR,
      ubo_layout_bindings);

  VK_TIE_ERR_PROP(descriptor_layout_,
                  device.createDescriptorSetLayout(dl_create_info));

  VK_NS::PipelineLayoutCreateInfo layout_create({},
                                                descriptor_layout_);
  std::tie(res, pipeline_layout_) =
      device.createPipelineLayout(layout_create);
  if (res != VK_NS::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  std::array dynamic_states = {VK_NS::DynamicState::eViewport,
                               VK_NS::DynamicState::eScissor};
  VK_NS::PipelineDynamicStateCreateInfo dynamic_state({},
                                                      dynamic_states);
  auto const input_bindings =
      vertex_shader_module.get_vertex_input_bindings();
  auto const input_attributes =
      vertex_shader_module.get_vertex_input_attributes();

  VK_NS::PipelineVertexInputStateCreateInfo vertex_input_info{
      {}, input_bindings, input_attributes};

  VK_NS::PipelineInputAssemblyStateCreateInfo input_assembly(
      {}, VK_NS::PrimitiveTopology::eTriangleList, false);

  VK_NS::Viewport viewport{0,
                           0,
                           static_cast<float>(size.width),
                           static_cast<float>(size.height),
                           0,
                           1};
  VK_NS::Rect2D scissor{{0, 0}, size};
  VK_NS::PipelineViewportStateCreateInfo viewport_state{
      {}, viewport, scissor};

  VK_NS::PipelineRasterizationStateCreateInfo rasterization(
      {}, false, false, VK_NS::PolygonMode::eFill,
      VK_NS::CullModeFlagBits::eNone,
      VK_NS::FrontFace::eCounterClockwise, false, {}, {}, {}, 1.0f);

  VK_NS::PipelineMultisampleStateCreateInfo multisample{
      {}, VK_NS::SampleCountFlagBits::e1, false};

  VK_NS::PipelineColorBlendAttachmentState colour_blend_attachment{
      false,
      VK_NS::BlendFactor::eSrcAlpha,
      VK_NS::BlendFactor::eOneMinusSrcAlpha,
      VK_NS::BlendOp::eAdd,
      VK_NS::BlendFactor::eOne,
      VK_NS::BlendFactor::eZero,
      VK_NS::BlendOp::eAdd};
  colour_blend_attachment.setColorWriteMask(
      VK_NS::ColorComponentFlagBits::eR |
      VK_NS::ColorComponentFlagBits::eG |
      VK_NS::ColorComponentFlagBits::eB |
      VK_NS::ColorComponentFlagBits::eA);
  VK_NS::PipelineColorBlendStateCreateInfo colour_blend(
      {}, false, VK_NS::LogicOp::eCopy, colour_blend_attachment,
      {0.0, 0.0, 0.0, 0.0});

  auto v_stage_create_info = VK_NS::PipelineShaderStageCreateInfo(
      {}, VK_NS::ShaderStageFlagBits::eVertex,
      vertex_shader_module.module, "main");
  auto f_stage_create_info = VK_NS::PipelineShaderStageCreateInfo(
      {}, VK_NS::ShaderStageFlagBits::eFragment,
      fragment_shader_module.module, "main");
  std::array shader_stages = {v_stage_create_info,
                              f_stage_create_info};

  auto create_info = VK_NS::GraphicsPipelineCreateInfo(
      {}, shader_stages, &vertex_input_info, &input_assembly, {},
      &viewport_state, &rasterization, &multisample, {},
      &colour_blend, &dynamic_state, pipeline_layout_, render_pass);

  std::tie(res, pipeline) =
      device.createGraphicsPipeline({}, create_info);
  if (res != VK_NS::Result::eSuccess)
    return tl::make_unexpected(make_error_code(res));

  return {};
}

}  // namespace wren::vk
