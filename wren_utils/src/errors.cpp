#include "errors.hpp"

#include <fmt/format.h>

#include <sstream>

namespace wren {}  // namespace wren

auto fmt::formatter<wren::Err>::format(wren::Err e,
                                       fmt::format_context& ctx) const
    -> decltype(ctx.out()) {
  std::stringstream ss;
  std::string fmt =
      fmt::format("Error: {} {}", e.error().category().name(),
                  e.error().message());

  ss << fmt;
  if (e.extra_msg().has_value()) {
    ss << "(" << e.extra_msg().value() << ")";
  }
  return fmt::formatter<std::string>::format(ss.str(), ctx);
}
