#pragma once

#include <array>
#include <filesystem>
#include <string_view>
#include <wren_utils/errors.hpp>

namespace wren::text {

static constexpr std::array DEFAULT_FONTS = {
    std::string_view{"Canterlla Regular"},
};

auto load_default_font() -> wren::expected<std::filesystem::path>;

}  // namespace wren::text
