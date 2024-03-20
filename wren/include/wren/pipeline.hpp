#pragma once

#include <memory>
#include <string>
#include <utility>

#include "shader.hpp"
#include "utils/errors.hpp"

namespace wren {

class Pipeline {
  using pipeline_t = std::shared_ptr<Pipeline>;

 public:
  static auto New(std::string const& name,
                  std::shared_ptr<Shader> const& shader)
      -> expected<std::shared_ptr<Pipeline>>;

 private:
  Pipeline(std::string  name,
           std::shared_ptr<Shader> const& shader)
      : name(std::move(name)), shader(shader) {}

  std::string name;

  std::shared_ptr<Shader> shader;
};

}  // namespace wren
