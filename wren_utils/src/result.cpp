#include "result.hpp"

#include <sstream>

namespace wren {}  // namespace wren

auto std::formatter<wren::Err>::format(wren::Err e,
                                       std::format_context& ctx) const
    -> decltype(ctx.out()) {
  std::stringstream ss;
  std::string fmt = std::format("Error: {} {}", e.error().category().name(),
                                e.error().message());

  ss << fmt;
  if (e.extra_msg().has_value()) {
    ss << "(" << e.extra_msg().value() << ")";
  }
  return std::formatter<std::string>::format(ss.str(), ctx);
}
