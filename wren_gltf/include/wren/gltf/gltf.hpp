#pragma once

#include <filesystem>

namespace wren::gltf {

auto load_mesh(const std::filesystem::path& path) -> void;

}
