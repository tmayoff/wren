#pragma once

#include <boost/describe.hpp>
#include <cstdint>
#include <span>
#include <spirv/unified1/spirv.hpp>
#include <string>
#include <vector>

#include "wren/utils/binary_reader.hpp"

namespace wren::reflect {

struct EntryPoint {
  std::string name;
  spv::ExecutionModel execution_model;
  uint32_t entry_point;
};

class Reflect {
 public:
  Reflect(const std::span<const std::byte>& spirv);

  [[nodiscard]] auto magic_number() const { return magic_number_; }

  [[nodiscard]] auto entry_points() const { return entry_points_; }

 private:
  auto parse_op_code(utils::BinaryReader& reader, uint32_t wordcount,
                     const spv::Op& opcode) -> uint32_t;

  uint32_t magic_number_;
  uint32_t version_;
  uint32_t generator_;
  uint32_t bound_;

  std::vector<EntryPoint> entry_points_;
};

}  // namespace wren::reflect
