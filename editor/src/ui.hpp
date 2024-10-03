#pragma once

#include <wren_utils/errors.hpp>

namespace vk {
class CommandBuffer;
}

namespace wren {
class Context;
}

namespace editor::ui {

auto init(const std::shared_ptr<wren::Context>& context)
    -> wren::expected<void>;

auto begin() -> void;
auto end() -> void;

void flush(const ::vk::CommandBuffer& cmd);

}  // namespace editor::ui
