#pragma once

#include <filesystem>

#include "mesh.hpp"

namespace wren {

DEFINE_ERROR("MeshLoader", MeshLoaderError, ExtensionNotSupported)

auto load_mesh(const std::filesystem::path& mesh_path) -> expected<Mesh>;

}  // namespace wren
