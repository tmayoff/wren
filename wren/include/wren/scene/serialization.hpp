#pragma once

#include <filesystem>

#include "scene.hpp"
#include "wren_utils/errors.hpp"

namespace wren::scene {

auto serialize(const std::shared_ptr<Scene>& scene,
               const std::filesystem::path& out_file) -> expected<void>;

}
