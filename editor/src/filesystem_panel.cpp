#include "filesystem_panel.hpp"

void render_filesystem_panel(const std::filesystem::path& project_root) {
  ImGui::Begin("Filesystem");

  ImGui::Text("Project root: %s", project_root.c_str());

  for (const auto& file : std::filesystem::directory_iterator(project_root)) {
    ImGui::Text("%s", file.path().c_str());
  }

  ImGui::End();
}
