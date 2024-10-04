#pragma once

#include <array>
#include <queue>

#include "component.hpp"
#include "entity.hpp"

namespace wren::ecs {

class Manager {
 public:
  Manager() {
    for (Entity entity = 0; entity < kMaxEntities; ++entity) {
      availableEntities_.push(entity);
    }
  }

 private:
  std::queue<Entity> availableEntities_;
  std::array<Signature, kMaxEntities> signatures_{};
  uint32_t living_entity_count_ = 0;
};

}  // namespace wren::ecs
