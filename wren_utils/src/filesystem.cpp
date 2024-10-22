#include "filesystem.hpp"

#include "errors.hpp"

namespace wren::utils::fs {

auto file_exists_in_dir(const std::filesystem::path& root,
                        const std::filesystem::path& file)
    -> expected<std::filesystem::path> {
  if (std::filesystem::exists(root / file)) {
    return root / file;
  }

  for (const auto& dir : std::filesystem::recursive_directory_iterator(root)) {
    if (std::filesystem::exists(dir / file)) {
      return dir / file;
    }
  }

  return std::unexpected(
      std::make_error_code(std::errc::no_such_file_or_directory));
}

auto read_file_to_string(const std::filesystem::path& path) -> std::string {
  std::ifstream file(path);
  std::stringstream result;
  result << file.rdbuf();
  return result.str();
}

auto read_file_to_bin(const std::filesystem::path& path)
    -> std::vector<uint8_t> {
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
