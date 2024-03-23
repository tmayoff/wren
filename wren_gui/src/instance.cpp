#include "instance.hpp"

namespace wren::gui {

Instance::Instance() {}

void Instance::draw(vk::CommandBuffer const& cmd) {}

auto BeginWindow() { return true; }

auto EndWindow() { return true; }

}  // namespace wren::gui
