#include "wren/reflect/reflect.hpp"

#include <spdlog/spdlog.h>

#include <cstdint>
#include <spirv/unified1/spirv.hpp>
#include <wren/utils/binary_reader.hpp>

#include "wren/utils/macros.hpp"

namespace wren::reflect {

auto parse_string(utils::BinaryReader& reader) -> std::string;

Reflect::Reflect(const std::span<const std::byte>& spirv) {
  utils::BinaryReader reader(spirv);

  magic_number_ = reader.read<uint32_t>();
  version_ = reader.read<uint32_t>();
  generator_ = reader.read<uint32_t>();
  bound_ = reader.read<uint32_t>();
  reader.read<uint32_t>();

  // Read instructions
  while (!reader.at_end()) {
    const auto op = reader.read<uint32_t>();
    const auto wordcount =
        (op >> 16) & 0xFFFF;  // We already read the first word above
    const auto opcode = static_cast<spv::Op>(op & 0xFFFF);

    size_t skipped_words = 1;

    skipped_words += parse_op_code(reader, wordcount - skipped_words, opcode);
    for (size_t i = skipped_words; i < wordcount; ++i) {
      reader.read<uint32_t>();
    }
  }
}

auto parse_string(utils::BinaryReader& reader) -> std::string {
  std::string s;

  bool ended = false;

  while (true) {
    char c = reader.read<char>();
    s += c;
    if (c == '\0') ended = true;

    if (ended && s.size() % sizeof(uint32_t) == 0) break;
  }

  return s;
}

auto Reflect::parse_op_code(utils::BinaryReader& reader,
                            const uint32_t wordcount, const spv::Op& op)
    -> uint32_t {
  uint32_t skipped_words = 0;

  switch (op) {
    case spv::Op::OpCapability: {
      const auto cap = static_cast<spv::Capability>(reader.read<uint32_t>());
      ++skipped_words;
      break;
    }

    case spv::Op::OpEntryPoint: {
      EntryPoint entry;

      entry.execution_model =
          static_cast<spv::ExecutionModel>(reader.read<uint32_t>());
      ++skipped_words;

      entry.entry_point = reader.read<uint32_t>();
      ++skipped_words;

      entry.name = parse_string(reader);
      skipped_words += entry.name.size() / sizeof(uint32_t);

      const uint32_t id_count = wordcount - skipped_words;
      std::vector<uint32_t> ids;
      for (size_t i = 0; i < id_count; ++i) {
        ids.push_back(reader.read<uint32_t>());
        ++skipped_words;
      }

      entry_points_.push_back(entry);

      break;
    }
    default:
      break;
  }

  if (skipped_words > 0)
    WREN_ASSERT(skipped_words == wordcount, "OpCode not finished");
  return skipped_words;
}

}  // namespace wren::reflect
