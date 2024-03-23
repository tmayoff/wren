#pragma once

#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <wren_utils/enums.hpp>

namespace wren::Event {

DESCRIBED_ENUM(Category, WINDOW, KEYBOARD, MOUSE)

// NOLINTNEXTLINE
#define APPEND_EVENT_INFO(name, cat)                      \
  inline static const std::string_view debug_name = name; \
  inline static const Category category = cat;

struct WindowClose {
  APPEND_EVENT_INFO("WindowClose", Category::WINDOW);
};

struct WindowResized {
  WindowResized() = default;
  explicit WindowResized(std::pair<float, float> const &size)
      : width(size.first), height(size.second) {}

  float width = 0;
  float height = 0;

  APPEND_EVENT_INFO("WindowResized", Category::WINDOW)
};

class Dispatcher {
 public:
  template <typename T>
  void dispatch(T &&value) const;
  template <typename T>
  void on(std::function<void(T const &t)> cb);

 private:
  std::unordered_map<size_t, std::vector<std::function<void(void *)>>>
      handlers;
};

template <typename Type>
void Dispatcher::dispatch(Type &&value) const {
  auto const id = typeid(Type{}).hash_code();
  if (handlers.contains(id)) {
    for (auto &cb : handlers.at(id)) {
      if (cb) cb(&value);
    }
  }
}

template <typename Type>
void Dispatcher::on(std::function<void(Type const &)> func) {
  auto const id = typeid(Type{}).hash_code();
  handlers[id].push_back([func = std::move(func)](void *value) {
    func(*static_cast<Type *>(value));
  });
}

}  // namespace wren::Event
