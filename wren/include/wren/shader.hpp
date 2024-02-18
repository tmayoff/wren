#pragma once

#include "vulkan/vulkan.hpp"
#include <shaderc/shaderc.h>
#include <string>
#include <system_error>
#include <tl/expected.hpp>
#include <wren/utils/device.hpp>

namespace wren {

struct ShaderModule {
  std::vector<uint32_t> spirv;
  vk::ShaderModule module;

  void load_reflection_info();
};

class Shader {
public:
  static auto Create(const vulkan::Device &device,
                     const std::string &vertex_shader,
                     const std::string &fragment_shader)
      -> tl::expected<Shader, std::error_code>;

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

private:
  ShaderModule vertex_shader_module;
  ShaderModule fragment_shader_module;
};

} // namespace wren
