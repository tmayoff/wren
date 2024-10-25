#pragma once

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <memory>
#include <optional>
#include <wren/scene/components/mesh.hpp>
#include <wren/scene/components/transform.hpp>
#include <wren/scene/entity.hpp>
#include <wren/scene/scene.hpp>

#include "context.hpp"

namespace editor {

void render_inspector_panel(
    const editor::Context& ctx,
    const std::shared_ptr<wren::scene::Scene>& scene,
    const std::optional<flecs::entity>& selected_entity);

}
