#include "wren/shader_reflection/parser.hpp"

#include <spdlog/spdlog.h>

#include <spirv/1.2/spirv.hpp>

namespace wren::spirv {

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

    switch (op) {
      case spv::Op::OpEntryPoint: {
        const auto execution_model =
            static_cast<spv::ExecutionModel>(*(arg_it++));

        arg_it++;  // const auto entry_point_id = *(arg_it++);
        const std::string entry_point_name =
            string_literal({arg_it, arguments.end()});
        entry_points_.push_back({execution_model, entry_point_name});

        break;
      }

      default:
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
      if (c == '\0') done = true;
      literal.push_back(static_cast<char>(c));
    }

    if (done) break;
  }

  return literal;
}

}  // namespace wren::spirv
