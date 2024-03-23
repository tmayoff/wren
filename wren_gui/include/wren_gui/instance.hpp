#pragma once

#include <vulkan/vulkan.hpp>

namespace wren::gui {

class Instance {
 public:
  Instance();

  void draw(VK_NS::CommandBuffer const& cmd);

  auto BeginWindow() -> bool;
  auto EndWindow() -> bool;
};

}  // namespace wren::gui
