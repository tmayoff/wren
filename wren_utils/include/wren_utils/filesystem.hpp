#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

namespace wren::utils::fs {

auto read_file_to_string(const std::filesystem::path& path) {
  std::ifstream file(path);
  std::string result;
  file >> result;
  return result;
}

auto read_file_to_bin(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);

  std::streampos size;
  file.seekg(0, std::ios::end);
  size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<uint8_t> buf;
  buf.reserve(size);

  buf.insert(buf.begin(), std::istreambuf_iterator<char>(file),
             std::istreambuf_iterator<char>());

  return buf;
}

}  // namespace wren::utils::fs
