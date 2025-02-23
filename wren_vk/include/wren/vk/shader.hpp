#pragma once

#include <wren/reflect/spirv_reflect.h>
// #include <wren/reflect/spirv_reflect.h>

#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
#include <wren/math/vector.hpp>
#include <wren/utils/result.hpp>

#include "wren/reflect/reflect.hpp"

namespace wren::reflect {
using spirv_t = std::vector<uint32_t>;
}

namespace wren::vk {

DESCRIBED_ENUM(ShaderType, Vertex, Fragment);

class ShaderModule {
 public:
  ShaderModule() = default;
  ShaderModule(reflect::spirv_t spirv, const ::vk::ShaderModule &module);

  [[nodiscard]] auto module() const -> ::vk::ShaderModule { return module_; }

  [[nodiscard]] auto get_entry_point(ShaderType type) const -> std::string;

  [[nodiscard]] auto get_vertex_input_bindings() const
      -> std::vector<::vk::VertexInputBindingDescription>;

  [[nodiscard]] auto get_vertex_input_attributes() const
      -> std::vector<::vk::VertexInputAttributeDescription>;

  [[nodiscard]] auto get_descriptor_set_layout_bindings() const
      -> std::vector<::vk::DescriptorSetLayoutBinding>;

 private:
  reflect::spirv_t spirv_;
  ::vk::ShaderModule module_;
  std::shared_ptr<spv_reflect::ShaderModule> reflection_;
  reflect::Reflect reflect_;
};

class Shader {
 public:
  using Ptr = std::shared_ptr<Shader>;

  static auto create(const ::vk::Device &device,
                     const std::string &vertex_shader,
                     const std::string &fragment_shader) -> expected<Ptr>;

  static auto create(const ::vk::Device &device,
                     const std::filesystem::path &shader_path) -> expected<Ptr>;

  static auto create(const ::vk::Device &device,
                     const std::span<const uint32_t> spirv_data)
      -> expected<Ptr>;

  // TODO compile slang shaders?
  //  static auto compile_shader(const ::vk::Device &device,
  //                             const shaderc_shader_kind &shader_kind,
  //                             const std::string &filename,
  //                             const std::string &shader_source)
  //      -> wren::expected<ShaderModule>;

  [[nodiscard]] auto get_pipeline() const { return pipeline_; }
  [[nodiscard]] auto pipeline_layout() const { return pipeline_layout_; }
  [[nodiscard]] auto descriptor_layout() const { return descriptor_layout_; }

  void fragment_shader(const ShaderModule &fragment) {
    fragment_shader_module_ = fragment;
  }

  void vertex_shader(const ShaderModule &vertex) {
    vertex_shader_module_ = vertex;
  }

  auto create_graphics_pipeline(const ::vk::Device &device,
                                const ::vk::RenderPass &render_pass,
                                const math::Vec2f &size, bool depth)
      -> expected<void>;

 private:
  static auto read_wren_shader_file(const std::filesystem::path &path)
      -> expected<std::map<ShaderType, std::string>>;

  ::vk::DescriptorSetLayout descriptor_layout_;
  ::vk::PipelineLayout pipeline_layout_;
  ::vk::Pipeline pipeline_;

  ShaderModule vertex_shader_module_;
  ShaderModule fragment_shader_module_;
};

}  // namespace wren::vk
