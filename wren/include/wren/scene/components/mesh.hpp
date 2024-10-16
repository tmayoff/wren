#pragma once

#include <filesystem>
#include <wren/mesh.hpp>
#include <wren/mesh_loader.hpp>

namespace wren::scene::components {

struct MeshRenderer {
  Mesh mesh;

  std::filesystem::path mesh_file;

  auto update_mesh(const std::filesystem::path& project_root,
                   const std::filesystem::path& mesh_path) -> expected<void> {
    mesh_file = mesh_path;

    TRY_RESULT(mesh, load_mesh(project_root / mesh_path));

    return {};
  }
};

}  // namespace wren::scene::components
