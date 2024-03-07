#include "wren_reflect/parser.hpp"

#include <cstring>
#include <spirv/1.2/spirv.hpp>

namespace wren::reflect {

template <typename T>
auto to_bytes(T value) -> std::vector<uint8_t> {
  std::vector<uint8_t> bytes(sizeof(T));
  std::memcpy(bytes.data(), &value, sizeof(T));
  return bytes;
}

Parser::Parser(spirv_t spirv) : spirv(std::move(spirv)) {
  load_reflection_info();
}

void Parser::load_reflection_info() {
  auto it = spirv.begin();

  it++;                              // Skip magic number
  version = *(it++);                 // Skip version number
  generator_magic_number = *(it++);  // skip generator's magic number
  bound = *(it++);
  it++;  // skip reserved

  while (it != spirv.end()) {
    const auto instruction = *it;
    const auto length = instruction >> 16;
    const auto op = instruction & 0x0ffffu;

    const std::span<uint32_t> arguments(it + 1, it + length);
    auto arg_it = arguments.begin();

    switch (static_cast<spv::Op>(op)) {
      case spv::Op::OpEntryPoint: {
        const auto execution_model =
            static_cast<spv::ExecutionModel>(*(arg_it++));

        arg_it++;  // const auto entry_point_id = *(arg_it++);
        const std::string entry_point_name =
            string_literal({arg_it, arguments.end()});
        entry_points_.push_back({execution_model, entry_point_name});

        break;
      }

      case spv::Op::OpName: {
        const auto target_id = *(arg_it++);
        const auto op_name =
            string_literal({arg_it, arguments.end()});
        op_names_.insert({target_id, op_name});
        break;
      }

      default:
        const auto id = *arg_it;
        arg_it++;
        const auto type = static_cast<spv::Op>(*(arg_it));
        SpvType t{id, type};

        if (arg_it != arguments.end()) {
          t.parent_type = *arg_it;
          arg_it++;
        }

        if (arg_it != arguments.end()) {
          if (type == spv::Op::OpVariable) {
            t.storage_class = static_cast<spv::StorageClass>(*arg_it);
          }
        }

        types_.push_back(t);
        break;
    }

    it += length;
  }
}

auto Parser::string_literal(const std::span<uint32_t> &literal_set)
    -> std::string {
  std::string literal = "";

  auto it = literal_set.begin();
  while (it != literal_set.end()) {
    const uint32_t arg = *(it++);
    auto chars = to_bytes(arg);
    bool done = false;
    for (const auto &c : chars) {
      if (c == '\0') {
        done = true;
        break;
      }
      literal.push_back(static_cast<char>(c));
    }

    if (done) break;
  }

  return literal;
}

}  // namespace wren::reflect
