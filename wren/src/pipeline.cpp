#include "wren/pipeline.hpp"

namespace wren {

auto Pipeline::New(std::string const& name,
                   std::shared_ptr<Shader> const& shader)
    -> expected<pipeline_t> {}

}  // namespace wren
