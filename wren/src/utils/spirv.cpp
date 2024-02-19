#include "spirv_cross_c.h"
#include "wren/utils/spirv_cross.hpp"

namespace wren::spirv {

CompilerGLSL::CompilerGLSL(const std::span<uint32_t> &spirv) {
  //spvc_context_create(&context);
  //spvc_context_parse_spirv(context, spirv.data(), spirv.size_bytes(), &ir);

  //spvc_context_create_compiler(context, SPVC_BACKEND_GLSL, ir,
   //                            SPVC_CAPTURE_MODE_TAKE_OWNERSHIP, &compiler);
}

auto CompilerGLSL::get_entry_point_and_shader_stages()
    -> std::vector<std::string> {
 // size_t num_entry_points = 0;

 // spvc_compiler_get_entry_points(compiler, nullptr, &num_entry_points);

 // std::span<const spvc_entry_point*> entry_points;
 // auto result = spvc_compiler_get_entry_points(compiler, entry_points.data(),
 //                                              &num_entry_points);
 // if (result != spvc_result::SPVC_SUCCESS) {
 //     return {};
 // }

  return {};
}
  

} // namespace wren::spirv
