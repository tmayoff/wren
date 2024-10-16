#pragma once

#include <memory>
#include <string>
#include <utility>
#include <wren/utils/errors.hpp>
#include <wren/vk/shader.hpp>

namespace wren {

class Pipeline {
  using pipeline_t = std::shared_ptr<Pipeline>;

 public:
  // static auto New(std::string const& name,
  //                 std::shared_ptr<vk::Shader> const& shader)
  //     -> expected<std::shared_ptr<Pipeline>>;

 private:
  Pipeline(std::string name, const std::shared_ptr<vk::Shader>& shader)
      : name_(std::move(name)), shader_(shader) {}

  std::string name_;

  std::shared_ptr<vk::Shader> shader_;
};

}  // namespace wren
