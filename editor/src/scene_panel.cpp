#include "scene_panel.hpp"

#include <imgui.h>

#include <wren/scene/components/tag.hpp>

void render_scene_panel(const std::shared_ptr<wren::scene::Scene> &scene,
                        entt::entity &selected_entity) {
  ImGui::Begin("Scene");

  const auto &entities = scene->registry().view<wren::scene::components::Tag>();
  ImGui::Text("Entity count: %zu", entities.size());

  auto entity_node_flags =
      ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;

  for (const auto &[entity, tag] : entities.each()) {
    if (entity == selected_entity) {
      entity_node_flags |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(tag.tag.c_str(), entity_node_flags)) {
      // Set the selected entity
      if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        selected_entity = entity;

      ImGui::TreePop();
    }
  }

  ImGui::End();
}
