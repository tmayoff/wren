#include <backward.hpp>
#include <filesystem>
#include <memory>
#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/graph.hpp>
#include <wren/shaders/mesh.hpp>
#include <wren/utils/format.hpp>

import editor;

const backward::SignalHandling kSh;

auto main(int argc, char** argv) -> int {
  spdlog::set_level(spdlog::level::debug);

  // Fix this warning
  std::span args(argv + 1, argc - 1);

  spdlog::debug("launching editor");
  spdlog::debug("args:");
  for (const auto arg : args) {
    spdlog::debug("\t{}", arg);
  }

  std::optional<std::filesystem::path> project_path;
  if (!args.empty()) {
    std::filesystem::path p(args.front());
    if (std::filesystem::exists(p)) {
      spdlog::info("Loading project {}", args.front());
      project_path = p;
    }
  }

  const auto& err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}", err.error().message());
    return EXIT_FAILURE;
  }

  auto app = err.value();

  // TODO Build 3D rendergraph, be able to check resources if non
  // match the output framebuffer add a pass specifically for
  // transitioning into the presentable layout

  const auto res = editor::Editor::create(app, project_path.value());
  if (!res.has_value()) {
    spdlog::error("{}", res.error());
    return EXIT_FAILURE;
  }
  const auto editor = res.value();

  app->run();

  return EXIT_SUCCESS;
}
