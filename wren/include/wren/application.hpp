#pragma once

#include <spdlog/spdlog.h>

#include <memory>
#include <tl/expected.hpp>

#include "context.hpp"
#include "wren/renderer.hpp"

namespace wren {

enum class CallbackPhase { Startup, Update, Shutdown };

using phase_cb_t = std::function<void()>;

class Application {
 public:
  enum class errors {};

  static auto Create(std::string const &application_name)
      -> expected<std::shared_ptr<Application>>;

  auto context() -> std::shared_ptr<Context> const & { return ctx; }
  void run();

  void add_callback_to_phase(CallbackPhase phase,
                             phase_cb_t const &cb);

 private:
  explicit Application(std::shared_ptr<Context> const &ctx,
                       std::shared_ptr<Renderer> const &renderer);

  std::shared_ptr<Context> ctx;
  std::shared_ptr<Renderer> renderer;

  std::vector<phase_cb_t> startup_phase;
  std::vector<phase_cb_t> update_phase;
  std::vector<phase_cb_t> shutdown_phase;

  bool running;
};

}  // namespace wren
