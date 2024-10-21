#include "assets/manager.hpp"

#include <wren/utils/filesystem.hpp>

namespace wren::assets {

auto Manager::find_asset(const std::filesystem::path& asset_path) const
    -> expected<std::filesystem::path> {
  // Check the project directory first
  if (project_path_.has_value()) {
    const auto res =
        utils::fs::file_exists_in_dir(project_path_.value(), asset_path);
    if (res.has_value()) {
      return res.value();
    }
  }

  // Check the rest of the asset paths
  for (const auto& asset_dir : asset_paths_) {
    const auto res = utils::fs::file_exists_in_dir(asset_dir, asset_path);
    if (res.has_value()) {
      return res.value();
    }
  }

  return tl::make_unexpected(
      std::make_error_code(std::errc::no_such_file_or_directory));
}

}  // namespace wren::assets
