#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <backward.hpp>
#include <memory>
#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/graph.hpp>
#include <wren/shaders/mesh.hpp>

#include "editor.hpp"

backward::SignalHandling const sh;

auto main() -> int {
  auto const &err = wren::Application::Create("Editor");
  if (!err.has_value()) {
    spdlog::error("failed to create application: {}",
                  err.error().message());
    return EXIT_FAILURE;
  }

  auto app = err.value();

  // TODO Build 3D rendergraph, be able to check resources if non
  // match the output framebuffer add a pass specifically for
  // transitioning into the presentable layout

  auto const res = Editor::create(app);
  if (!res.has_value()) {
    return EXIT_FAILURE;
  }
  auto const editor = res.value();

  app->run();

  return EXIT_SUCCESS;
}
