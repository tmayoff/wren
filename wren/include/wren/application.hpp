#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <tl/expected.hpp>

#include "context.hpp"
#include "wren/renderer.hpp"

namespace wren {

class Application {
 public:
  enum class errors {};

  static auto Create(std::string const &application_name)
      -> tl::expected<std::shared_ptr<Application>, std::error_code>;

  auto context() -> std::shared_ptr<Context> const & { return ctx; }
  void run();

 private:
  explicit Application(std::shared_ptr<Context> const &ctx,
                       std::shared_ptr<Renderer> const &renderer);

  std::shared_ptr<Context> ctx;
  std::shared_ptr<Renderer> renderer;

  bool running;
};

}  // namespace wren
