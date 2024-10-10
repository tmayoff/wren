#pragma once

#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <wren_utils/enums.hpp>

#include "keycode.hpp"

namespace wren::event {

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
  explicit WindowResized(const std::pair<float, float> &size)
      : width(size.first), height(size.second) {}

  float width = 0;
  float height = 0;

  APPEND_EVENT_INFO("WindowResized", Category::WINDOW)
};

struct MouseMoved {
  float x = 0;
  float y = 0;
  bool relative = false;

  APPEND_EVENT_INFO("MouseMoved", Category::MOUSE)
};

struct MouseWheel {
  float x = 0;
  float y = 0;

  APPEND_EVENT_INFO("MouseWheel", Category::MOUSE)
};

struct MouseButtonDown {
  MouseCode button;
  APPEND_EVENT_INFO("MouseButtonDown", Category::MOUSE)
};

struct MouseButtonUp {
  MouseCode button;
  APPEND_EVENT_INFO("MouseButtonUp", Category::MOUSE)
};

struct KeyDown {
  KeyCode key;
  APPEND_EVENT_INFO("KeyDown", Category::KEYBOARD)
};

struct KeyUp {
  KeyCode key;
  APPEND_EVENT_INFO("KeyUp", Category::KEYBOARD)
};

struct KeyTyped {
  std::string_view text;
  APPEND_EVENT_INFO("KeyTyped", Category::KEYBOARD);
};

class Dispatcher {
 public:
  template <typename T>
  void dispatch(T &&value) const;
  template <typename T>
  void on(std::function<void(const T &t)> cb);

 private:
  std::unordered_map<size_t, std::vector<std::function<void(void *)>>>
      handlers_;
};

template <typename Type>
// NOLINTNEXTLINE
void Dispatcher::dispatch(Type &&value) const {
  const auto id = typeid(Type{}).hash_code();
  if (handlers_.contains(id)) {
    for (auto &cb : handlers_.at(id)) {
      if (cb) cb(&value);
    }
  }
}

template <typename Type>
void Dispatcher::on(std::function<void(const Type &)> func) {
  const auto id = typeid(Type{}).hash_code();
  handlers_[id].push_back([func = std::move(func)](void *value) {
    func(*static_cast<Type *>(value));
  });
}

}  // namespace wren::event
