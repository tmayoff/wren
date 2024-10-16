#pragma once

#include <memory>
#include <optional>
#include <wren/scene/scene.hpp>

#include "context.hpp"

void render_inspector_panel(
    const editor::Context& ctx,
    const std::shared_ptr<wren::scene::Scene>& scene,
    const std::optional<flecs::entity>& selected_entity);
