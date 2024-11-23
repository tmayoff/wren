#include <spdlog/spdlog.h>

#include <cstdlib>
#include <wren/application.hpp>

auto main() -> int {
  spdlog::set_level(spdlog::level::debug);

  // Fix this warning
  // std::span args(argv + 1, argc - 1);

  // spdlog::debug("launching editor");
  // spdlog::debug("args:");
  // for (const auto arg : args) {
  //   spdlog::debug("\t{}", arg);
  // }

  // std::optional<std::filesystem::path> project_path;
  // if (!args.empty()) {
  //   std::filesystem::path p(args.front());
  //   if (std::filesystem::exists(p)) {
  //     spdlog::info("Loading project {}", args.front());
  //     project_path = p;
  //   }
  // }

  const auto& err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}", err.error().message());
    return EXIT_FAILURE;
  }

  auto app = err.value();

  // TODO setup render graph

  app->run();

  return EXIT_SUCCESS;
}