#pragma once

#include <memory>
#include <optional>
#include <wren/scene/scene.hpp>

void render_scene_panel(const std::shared_ptr<wren::scene::Scene>& scene,
                        std::optional<flecs::entity>& selected_entity);
