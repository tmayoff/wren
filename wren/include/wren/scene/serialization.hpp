#pragma once

#include <filesystem>

#include "scene.hpp"
#include "wren_utils/errors.hpp"

namespace wren::scene {

auto serialize(const Scene& scene, const std::filesystem::path& out_file)
    -> expected<void>;

}
