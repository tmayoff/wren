module;

#include <imgui.h>

#include "context.hpp"

export module editor:filesystem;

export void render_filesystem_panel(const editor::Context& ctx) {
  ImGui::Begin("Filesystem");

  ImGui::Text("Project root: %s", ctx.project_path.c_str());

  for (const auto& file :
       std::filesystem::directory_iterator(ctx.project_path)) {
    const auto f = std::filesystem::relative(file, ctx.project_path);

    int flags = ImGuiTreeNodeFlags_SpanAvailWidth;
    if (!file.is_directory()) flags |= ImGuiTreeNodeFlags_Leaf;

    if (ImGui::TreeNodeEx(f.c_str(), flags)) {
      // Get children

      ImGui::TreePop();
    }

    auto filename = f.string();

    if (ImGui::BeginDragDropSource()) {
      ImGui::SetTooltip("%s", filename.c_str());
      ImGui::SetDragDropPayload("FILESYSTEM_BROWSER", filename.c_str(),
                                filename.size() + 1, ImGuiCond_Once);
      ImGui::EndDragDropSource();
    }
  }

  ImGui::End();
}
