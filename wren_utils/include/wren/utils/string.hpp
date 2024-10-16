#pragma once

#include <string>
#include <vector>

namespace wren::utils {

auto split(const std::string& input, const char c) -> std::vector<std::string>;

}  // namespace wren::utils
