#include "inspector_panel.hpp"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <wren/scene/components/mesh.hpp>
#include <wren/scene/components/tag.hpp>
#include <wren/scene/components/transform.hpp>
#include <wren/scene/entity.hpp>

void draw_component(wren::scene::components::MeshRenderer& mesh_renderer);
void draw_component(wren::scene::components::Transform& transform);
void draw_component(const std::string_view& tag, wren::math::Vec3f& vec);

void render_inspector_panel(
    const std::shared_ptr<wren::scene::Scene>& scene,
    const std::optional<flecs::entity>& selected_entity) {
  ImGui::Begin("Inspector");

  if (!selected_entity.has_value()) {
    ImGui::End();
    return;
  }

  wren::scene::Entity entity{selected_entity.value(), scene};

  // Edit name
  auto name = std::string(selected_entity->name());
  ImGui::InputText("Name", &name);
  selected_entity->set_name(name.c_str());

  // Iterate all components
  selected_entity->each([selected_entity](flecs::id id) {
    if (id == selected_entity->world()
                  .component<wren::scene::components::Transform>()
                  .id()) {
      draw_component(
          *selected_entity->get_mut<wren::scene::components::Transform>());
    } else {
      ImGui::Text("Can't inspect component type: %s", id.str().c_str());
    }
  });

  ImGui::End();
}

void draw_component(wren::scene::components::MeshRenderer& mesh_renderer) {
  ImGui::Text("Mesh Renderer");
}

void draw_component(wren::scene::components::Transform& transform) {
  draw_component("pos", transform.position);
  draw_component("rot", transform.rotation);
  // draw_component(transform.scale);
}

void draw_component(const std::string_view& tag, wren::math::Vec3f& vec) {
  ImGui::PushItemWidth(80);
  ImGui::Text("X");
  ImGui::SameLine();
  float x = vec.x();
  if (ImGui::DragFloat(("##x" + std::string(tag.data())).c_str(), &x, 0.1F))
    vec.x(x);

  ImGui::SameLine();
  ImGui::Text("Y");
  ImGui::SameLine();
  float y = vec.y();
  if (ImGui::DragFloat(("##y" + std::string(tag.data())).c_str(), &y, 0.1F))
    vec.y(y);

  ImGui::SameLine();
  ImGui::Text("Z");
  ImGui::SameLine();
  float z = vec.z();
  if (ImGui::DragFloat(("##z" + std::string(tag.data())).c_str(), &z, 0.1F))
    vec.z(z);

  ImGui::PopItemWidth();
}
