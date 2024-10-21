#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <wren/utils/errors.hpp>

namespace wren::assets {

class Manager {
 public:
  Manager() = default;
  Manager(const std::vector<std::filesystem::path>& asset_paths,
          const std::optional<std::filesystem::path>& project_path = {})
      : asset_paths_(asset_paths), project_path_(project_path) {}

  [[nodiscard]] auto find_asset(const std::filesystem::path&) const
      -> expected<std::filesystem::path>;

 private:
  std::vector<std::filesystem::path> asset_paths_;
  std::optional<std::filesystem::path> project_path_;
};

}  // namespace wren::assets
