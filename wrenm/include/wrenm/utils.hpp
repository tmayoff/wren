#pragma once

#include <ostream>

#include "vector.hpp"
#include "matrix.hpp"

auto operator<<(std::ostream& os, wrenm::vec2f const& value)
    -> std::ostream&;

auto operator<<(std::ostream& os, wrenm::mat4f const& value)
    -> std::ostream&;
