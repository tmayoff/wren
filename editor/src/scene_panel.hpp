#pragma once

#include <memory>
#include <wren/scene/scene.hpp>

void render_scene_panel(const std::shared_ptr<wren::scene::Scene>& scene,
                        entt::entity& selected_entity);
