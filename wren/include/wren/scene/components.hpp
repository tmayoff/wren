#pragma once

#include <entt/entity/fwd.hpp>
#include <functional>

#include "scene.hpp"

namespace wren::scene {

void iterate_known_components(const Scene& scene, entt::entity entity,
                              std::function<void()>);

}
