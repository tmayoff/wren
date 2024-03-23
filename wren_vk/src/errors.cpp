#include "errors.hpp"

#include <vulkan/vulkan_to_string.hpp>

template <>
auto enum_to_string<vk::Result>(vk::Result e) -> std::string {
  return vk::to_string(e);
}
