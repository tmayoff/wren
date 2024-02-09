#include <cstdlib>

#include <wren/application.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/std.h>

auto main() -> int {
  const auto &err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("{}", err.error().message());
    return EXIT_FAILURE;
  }
  auto app = err.value();

  app->run();

  return EXIT_SUCCESS;
}
