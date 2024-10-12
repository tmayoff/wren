#pragma once

#include <memory>

namespace wren::ecs {

class Component {
 public:
  using Ptr = std::shared_ptr<Component>;
  Component() = default;
  Component(const Component &) = default;
  Component(Component &&) = delete;
  auto operator=(const Component &) -> Component & = default;
  auto operator=(Component &&) -> Component & = delete;
  virtual ~Component() = default;
};

}  // namespace wren::ecs
