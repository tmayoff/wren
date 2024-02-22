#pragma once

#include "wren/utils/vulkan_errors.hpp"
#include <spirv/1.2/spirv.hpp>
#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren::spirv {

inline auto get_vk_shader_stage(const spv::ExecutionModel &execution_model)
    -> tl::expected<vk::ShaderStageFlagBits, std::error_code> {
  switch (execution_model) {
  case spv::ExecutionModelVertex:
      return vk::ShaderStageFlagBits::eVertex;
  case spv::ExecutionModelTessellationControl:
      return vk::ShaderStageFlagBits::eTessellationControl;
  case spv::ExecutionModelTessellationEvaluation:
      return vk::ShaderStageFlagBits::eTessellationEvaluation;
  case spv::ExecutionModelGeometry:
      return vk::ShaderStageFlagBits::eGeometry;
  case spv::ExecutionModelFragment:
      return vk::ShaderStageFlagBits::eFragment;
  case spv::ExecutionModelGLCompute:
      return vk::ShaderStageFlagBits::eCompute;
  default:
      return tl::make_unexpected(
          std::make_error_code(std::errc::invalid_argument));
  }
}

} // namespace wren::spirv
