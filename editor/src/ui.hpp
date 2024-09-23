#pragma once

#include <wren/context.hpp>

namespace editor::ui {

auto init(std::shared_ptr<wren::Context> const& context)
    -> wren::expected<void>;

auto begin() -> void;
auto end() -> void;

void flush(::vk::CommandBuffer const& cmd);

}  // namespace editor::ui
