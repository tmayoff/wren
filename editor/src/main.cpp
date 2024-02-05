#include <cstdlib>

#include <wren/application.hpp>

int main() {
  const auto &err = wren::Application::Create();
  if (!err.has_value()) {
    return EXIT_FAILURE;
  }
  auto app = err.value();

  app.run();

  return EXIT_SUCCESS;
}
