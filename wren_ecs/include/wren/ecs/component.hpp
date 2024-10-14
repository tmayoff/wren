#pragma once

#include <memory>
#include <typeindex>
#include <vector>

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

template <typename T>
constexpr auto get_copmonent_id(T t) {
  return typeinfo(t);
}

using ComponentID = std::type_index;

using ComponentType = std::vector<ComponentID>;

}  // namespace wren::ecs
