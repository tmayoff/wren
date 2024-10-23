#pragma once

#include <filesystem>
#include <wren/utils/result.hpp>

#include "scene.hpp"

namespace wren::scene {

auto serialize(const std::shared_ptr<Scene>& scene,
               const std::filesystem::path& out_file) -> expected<void>;

}
