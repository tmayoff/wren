#pragma once

#include <bitset>
#include <cstdint>

namespace wren::ecs {

using ComponentType = uint8_t;
const ComponentType kMaxComponents = 32;

using Signature = std::bitset<kMaxComponents>;

}  // namespace wren::ecs
