#include "scene_panel.hpp"

#include <imgui.h>

#include <wren/scene/components/transform.hpp>

void render_scene_panel(const std::shared_ptr<wren::scene::Scene> &scene,
                        std::optional<flecs::entity> &selected_entity) {
  ImGui::Begin("Scene");

  ImGui::Text("Entity count: %d",
              scene->world().count<wren::scene::components::Transform>());

  auto q = scene->world()
               .query_builder<const wren::scene::components::Transform>()
               .build();
  q.each([&selected_entity](flecs::entity entity,
                            const wren::scene::components::Transform &) {
    const auto name = entity.name().size() == 0 ? "(unamed)" : entity.name();

    auto entity_node_flags =
        ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

    if (selected_entity.has_value() && entity == selected_entity) {
      entity_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(name, entity_node_flags)) {
      // Set the selected entity
      if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        selected_entity = entity;

      ImGui::TreePop();
    }
  });

  ImGui::End();
}
