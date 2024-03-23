#include "errors.hpp"

#include <vulkan/vulkan_to_string.hpp>

template <>
auto enum_to_string<VK_NS::Result>(VK_NS::Result e) -> std::string {
  return VK_NS::to_string(e);
}
