#pragma once

#include "utils/enums.hpp"
#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace wren::Event {

DESCRIBED_ENUM(Category, WINDOW, KEYBOARD, MOUSE)

struct Base {
  std::string_view name;
  Category category;
};

struct WindowClose : public Base {};

class Dispatcher {
public:
  template <typename T> void dispatch(T &&value) const;
  template <typename T> void on(std::function<void()> cb);

private:
  std::unordered_map<size_t, std::vector<std::function<void()>>> handlers;
};

template <typename Type> void Dispatcher::dispatch(Type &&value) const {
  const auto id = typeid(Type{}).hash_code();
  if (handlers.contains(id)) {
    for (auto &cb : handlers.at(id)) {
      if (cb)
        cb();
    }
  }
}

template <typename Type> void Dispatcher::on(std::function<void()> func) {
  const auto id = typeid(Type{}).hash_code();
  handlers[id].push_back([func = std::move(func)]() {
    // func(*static_cast<Type *>(value));
    func();
  });
}

} // namespace wren::Event
