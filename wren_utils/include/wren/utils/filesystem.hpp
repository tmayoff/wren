#pragma once

#include <filesystem>
#include <vector>

#include "result.hpp"

namespace wren::utils::fs {

auto file_exists_in_dir(const std::filesystem::path& root,
                        const std::filesystem::path& file)
    -> expected<std::filesystem::path>;

auto read_file_to_string(const std::filesystem::path& path) -> std::string;

auto read_file_to_bin(const std::filesystem::path& path)
    -> std::vector<uint8_t>;

}  // namespace wren::utils::fs
