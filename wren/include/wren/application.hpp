#pragma once

#include "context.hpp"
#include "tl/expected.hpp"
#include <memory>

namespace wren {

class Application {
public:
  enum class errors {};

  static auto Create() -> tl::expected<Application, std::error_code>;

  void run();

private:
  explicit Application(const std::shared_ptr<Context> &ctx);

  std::shared_ptr<Context> ctx;

  bool running ;
};

} // namespace wren
