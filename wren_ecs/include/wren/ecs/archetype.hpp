#pragma once

#include <cstdint>

#include "component.hpp"

namespace wren::ecs {

using ArchetypeID = uint64_t;

struct Archetype {
  ArchetypeID id;
  ComponentType type;
};

}  // namespace wren::ecs
