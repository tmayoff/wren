#include <spdlog/fmt/std.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>

auto main() -> int {
  const auto &err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }
  auto app = err.value();

  app->run();

  return EXIT_SUCCESS;
}
