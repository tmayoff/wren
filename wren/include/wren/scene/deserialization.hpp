#pragma once

#include <filesystem>

#include "scene.hpp"
#include "wren/utils/errors.hpp"

namespace wren::scene {

auto deserialize(const std::filesystem::path& project_root,
                 const std::filesystem::path& file,
                 const std::shared_ptr<Scene>& scene) -> expected<void>;

}
