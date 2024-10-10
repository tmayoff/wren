#pragma once

#include <wren_utils/errors.hpp>

namespace vk {
class CommandBuffer;
}

namespace wren {
struct Context;
}

namespace editor::ui {

auto init(const std::shared_ptr<wren::Context>& context)
    -> wren::expected<void>;

auto begin() -> void;
auto end() -> void;

void flush(const ::vk::CommandBuffer& cmd);

// void matrix() {
// ImGui::PushItemWidth(80);
// for (auto row = 0; row < 4; ++row) {
//   for (auto col = 0; col < 4; ++col) {
//     const std::string label = std::format("{}_{}", row, col);
//     ImGui::DragFloat(label.c_str(), &proj.at(col, row), 0.24f);
//     if (col < 3) {
//       ImGui::SameLine();
//     }
//   }
// }
// ImGui::PopItemWidth();
// }

}  // namespace editor::ui
