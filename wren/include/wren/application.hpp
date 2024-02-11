#pragma once

#include "context.hpp"
#include "wren/renderer.hpp"
#include <memory>
#include <spdlog/spdlog.h>
#include <tl/expected.hpp>

namespace wren {

class Application {
public:
  enum class errors {};

  static auto Create(const std::string &application_name)
      -> tl::expected<std::shared_ptr<Application>, std::error_code>;

  void run();

private:
  explicit Application(const std::shared_ptr<Context> &ctx,
                       const std::shared_ptr<Renderer> &renderer);

  std::shared_ptr<Context> ctx;
  std::shared_ptr<Renderer> renderer;

  bool running;
};

} // namespace wren
