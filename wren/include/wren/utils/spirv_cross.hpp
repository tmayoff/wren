#pragma once

#include "wren/utils/vulkan_errors.hpp"
#include <spirv_cross_c.h>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <memory>

namespace wren::spirv {

class CompilerGLSL {
public:
  CompilerGLSL(const CompilerGLSL &) = default;
  CompilerGLSL(CompilerGLSL &&) = delete;
  auto operator=(const CompilerGLSL &) -> CompilerGLSL & = default;
  auto operator=(CompilerGLSL &&) -> CompilerGLSL & = delete;
  CompilerGLSL(const std::span<uint32_t> &spirv);

  ~CompilerGLSL() { spvc_context_destroy(context); }

   auto get_entry_point_and_shader_stages() -> std::vector<std::string>; 
  
  private:
  spvc_context context = nullptr;
  spvc_parsed_ir ir = nullptr;
  spvc_compiler compiler = nullptr;
};

} // namespace wren::spirv
