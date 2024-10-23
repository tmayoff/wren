#pragma once

#include <filesystem>
#include <wren/utils/result.hpp>

#include "scene.hpp"

namespace wren::scene {

auto deserialize(const std::filesystem::path& project_root,
                 const std::filesystem::path& file,
                 const std::shared_ptr<Scene>& scene) -> expected<void>;

}
