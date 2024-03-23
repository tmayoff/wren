#include "instance.hpp"

namespace wren::gui {

Instance::Instance() {}

void Instance::draw(vk::CommandBuffer const& cmd) {}

auto Instance::BeginWindow() -> bool { return true; }

auto Instance::EndWindow() -> bool { return true; }

}  // namespace wren::gui
