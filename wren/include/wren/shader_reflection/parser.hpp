#pragma once

#include <cstdint>
#include <span>
#include <spirv/1.2/spirv.hpp>
#include <string>
#include <vector>

namespace wren::spirv {

struct EntryPoint {
  spv::ExecutionModel exeuction_model;
  std::string name;
};

using spirv_t = std::vector<uint32_t>;

class Parser {
 public:
  Parser(spirv_t spirv);

  [[nodiscard]] auto entry_points() const { return entry_points_; }

 private:
  void load_reflection_info();

  auto peek(spirv_t::iterator it) { return *(it + 1); }

  static auto string_literal(
      const std::span<uint32_t>& string_literal) -> std::string;

  spirv_t spirv;

  uint32_t version = 0;
  uint32_t generator_magic_number = 0;
  uint32_t bound = 0;

  std::vector<EntryPoint> entry_points_;
};

inline auto to_string(spv::ExecutionModel model) {
  switch (model) {
    case spv::ExecutionModelVertex:
      return "vertex";
    case spv::ExecutionModelTessellationControl:
      return "tessellation control";
    case spv::ExecutionModelTessellationEvaluation:
      return "tessellation evaluation";
    case spv::ExecutionModelGeometry:
      return "geometry";
    case spv::ExecutionModelFragment:
      return "fragment";
    case spv::ExecutionModelGLCompute:
      return "gl_compute";
    case spv::ExecutionModelKernel:
      return "kernel";
    case spv::ExecutionModelMax:
      return "unknown_max";
    default:
      return "unknown";
  }
}

}  // namespace wren::spirv
