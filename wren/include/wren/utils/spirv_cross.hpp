#pragma once

#include "wren/utils/vulkan_errors.hpp"
#include <spirv_cross/spirv_glsl.hpp>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

namespace wren::spirv {

auto get_vk_shader_stage(const spv::ExecutionModel &stage) -> tl::expected<vk::ShaderStageFlagBits, std::error_code>{
  switch (stage) {
  case spv::ExecutionModelVertex:
    return vk::ShaderStageFlagBits::eVertex;
   case spv::ExecutionModelFragment:
    return vk::ShaderStageFlagBits::eFragment;
  case spv::ExecutionModelTessellationControl:
  case spv::ExecutionModelTessellationEvaluation:
  case spv::ExecutionModelGeometry:
  case spv::ExecutionModelGLCompute:
  case spv::ExecutionModelKernel:
  case spv::ExecutionModelTaskNV:
  case spv::ExecutionModelMeshNV:
  case spv::ExecutionModelRayGenerationKHR:
  case spv::ExecutionModelIntersectionKHR:
  case spv::ExecutionModelAnyHitKHR:
  case spv::ExecutionModelClosestHitKHR:
  case spv::ExecutionModelMissKHR:
  case spv::ExecutionModelCallableKHR:
  case spv::ExecutionModelTaskEXT:
  case spv::ExecutionModelMeshEXT:
  case spv::ExecutionModelMax:
    return tl::make_unexpected(make_error_code(vk::Result::eErrorUnknown));
  }
}

} // namespace wren::spirv_cross
