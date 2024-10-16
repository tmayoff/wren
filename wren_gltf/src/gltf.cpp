#include "gltf.hpp"

#include <spdlog/spdlog.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <span>
#include <vector>

namespace wren::gltf {

enum class ChunkType {
  kJson = 0x4E4F534A,
  kBin = 0x004E4942,
};

auto load_json_chunk() {}

auto load_chunk() {}

auto load_mesh(const std::filesystem::path& path) -> void {
  // ==== Read file
  std::ifstream file(path, std::ios::binary);

  std::streampos size;
  file.seekg(0, std::ios::end);
  size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buf;
  buf.reserve(size);

  buf.insert(buf.begin(), std::istreambuf_iterator<char>(file),
             std::istreambuf_iterator<char>());

  // ===========================================

  std::span<uint32_t> data(reinterpret_cast<uint32_t*>(buf.data()),
                           buf.size() / sizeof(uint32_t));
  auto it = data.begin();

  const auto magic_number = *(it);
  ++it;
  assert(magic_number == 0x46546C67);

  const auto version = *it;
  ++it;
  const auto length = *it;
  ++it;
  spdlog::info("gltf version: {}, length: {}", version, length);

  while (it != data.end()) {
    const std::span<uint32_t> data(reinterpret_cast<uint32_t*>(buf.data()),
                                   buf.size() / sizeof(uint32_t));
    auto it = data.begin();

    const auto chunk_length = *it;
    ++it;
    const auto chunk_type = static_cast<ChunkType>(*it);
    ++it;

    auto chunk_start = std::distance(data.begin(), it) * sizeof(uint32_t);

    std::span<uint8_t> chunk_data(buf.begin() + chunk_start, chunk_length);
    if (chunk_type == ChunkType::kBin) {
      spdlog::debug("Chunk is binary data");
    } else if (chunk_type == ChunkType::kJson) {
      spdlog::debug("Chunk is json");

      auto json = nlohmann::json::parse(chunk_data.begin(), chunk_data.end());
      auto mesh_json = json["meshes"][0];

      std::cout << json.dump(4) << std::endl;
    }
  }
}

}  // namespace wren::gltf
