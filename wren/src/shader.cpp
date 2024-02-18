#include "wren/shader.hpp"
#include "tl/expected.hpp"
#include "vulkan/vulkan_structs.hpp"
#include "wren/utils/vulkan_errors.hpp"
#include "wren/window.hpp"
#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <system_error>

namespace wren {

void ShaderModule::load_reflection_info() {
  spirv_cross::CompilerGLSL glsl(spirv);
  const auto& resource = glsl.get_shader_resources();
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

} // namespace wren
