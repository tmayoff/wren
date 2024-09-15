#pragma once

#include <wren/context.hpp>

namespace editor::ui {

auto init(std::shared_ptr<wren::Context> const& context)
    -> wren::expected<void>;

auto begin() -> void;
auto end() -> void;

void flush(VK_NS::CommandBuffer const& cmd);

}  // namespace editor::ui
