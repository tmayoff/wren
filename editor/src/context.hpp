#pragma once

#include <filesystem>
#include <wren/assets/manager.hpp>

namespace editor {

struct Context {
  wren::assets::Manager asset_manager;
  std::filesystem::path project_path;
};

}  // namespace editor
