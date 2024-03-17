#pragma once

#include <ostream>

#include "vector.hpp"

auto operator<<(std::ostream& os, wrenm::vec2f const& value)
    -> std::ostream&;
