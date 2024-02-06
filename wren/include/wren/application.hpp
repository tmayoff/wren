#pragma once

#include "context.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

namespace wren {

class Application {
public:
  enum class errors {};

  static auto Create(const std::string &application_name)
      -> tl::expected<std::shared_ptr<Application>, std::error_code>;

  ~Application() { ctx->window.Shutdown(); }

  void run();

private:
  explicit Application(const std::shared_ptr<Context> &ctx);

  std::shared_ptr<Context> ctx;

  bool running;
};

} // namespace wren
