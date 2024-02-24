#pragma once

#include <shaderc/shaderc.h>

#include <memory>
#include <string>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "wren/shader_reflection/parser.hpp"
#include "wren/utils/device.hpp"

namespace wren {

using spirv_t = std::vector<uint32_t>;

struct ShaderModule {
  spirv_t spirv;
  vk::ShaderModule module;
  std::shared_ptr<spirv::Parser> parser;

  ShaderModule() = default;
  ShaderModule(spirv_t spirv, const vk::ShaderModule &module);

  [[nodiscard]] auto get_shader_stage_info() const
      -> vk::PipelineShaderStageCreateInfo;

  [[nodiscard]] auto get_vertex_input() const
      -> vk::PipelineVertexInputStateCreateInfo;
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

  void fragment_shader(const ShaderModule &fragment) {
    fragment_shader_module = fragment;
  }

  void vertex_shader(const ShaderModule &vertex) {
    vertex_shader_module = vertex;
  }

  auto reflect_graphics_pipeline() -> vk::GraphicsPipelineCreateInfo;

 private:
  ShaderModule vertex_shader_module;
  ShaderModule fragment_shader_module;
};

}  // namespace wren
