#pragma once

#include <memory>
#include <wren/scene/scene.hpp>

void render_inspector_panel(const std::shared_ptr<wren::scene::Scene>& scene,
                            const std::optional<entt::entity>& selected_entity);
