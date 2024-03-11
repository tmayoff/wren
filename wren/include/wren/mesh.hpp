#pragma once

#include <memory>
#include <wrenm/vector.hpp>

#include "buffer.hpp"

namespace wren {

struct Vertex {
  wrenm::vec2 pos;
  wrenm::vec3 colour;
};

class Mesh {
 private:
  std::unique_ptr<Buffer> vertex_buffer_;
};

}  // namespace wren
