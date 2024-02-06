#pragma once

#include <system_error>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

namespace wren {

class GraphicsContext {
public:
  static auto
  Create(const std::string &application_name,
         const std::vector<std::string_view> &requested_extensions = {},
         const std::vector<std::string_view> &requested_layers = {})
      -> tl::expected<GraphicsContext, std::error_code>;

  void Shutdown();

private:
  auto
  CreateInstance(const std::string &application_name,
                 const std::vector<std::string_view> &requested_extensions = {},
                 const std::vector<std::string_view> &requested_layers = {})
      -> tl::expected<void, std::error_code>;

  vk::Instance instance;
};

} // namespace wren
