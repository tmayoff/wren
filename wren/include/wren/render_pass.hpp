#pragma once

#include <memory>
#include <string>
#include <tl/expected.hpp>
#include <vulkan/vulkan.hpp>

#include "shader.hpp"

namespace wren {

struct RenderTarget {
  vk::Format format;
  vk::SampleCountFlagBits sample_count;
};

struct PassResources {
  std::shared_ptr<Shader> shader;
  std::vector<RenderTarget> render_targets;
};

class RenderPass {
 public:
  static auto Create(const vk::Device& device,
                     const std::string& name,
                     const PassResources& resources)
      -> tl::expected<RenderPass, std::error_code>;

 private:
  RenderPass(std::string name, PassResources resources)
      : name(std::move(name)), resources(std::move(resources)) {}

  std::string name;
  PassResources resources;
  vk::Pipeline pipeline;
};

}  // namespace wren
