#pragma once

#include <spdlog/spdlog.h>

#include <memory>

#include "context.hpp"
#include "wren/renderer.hpp"

namespace wren {

enum class CallbackPhase { Startup, Update, Shutdown };

using phase_cb_t = std::function<void()>;

class Application {
 public:
  enum class errors {};

  static auto Create(const std::string &application_name)
      -> expected<std::shared_ptr<Application>>;

  auto context() -> const std::shared_ptr<Context> & { return ctx; }
  void run();

  void add_callback_to_phase(CallbackPhase phase, const phase_cb_t &cb);

 private:
  explicit Application(const std::shared_ptr<Context> &ctx,
                       const std::shared_ptr<Renderer> &renderer);

  std::shared_ptr<Context> ctx;
  std::shared_ptr<Renderer> renderer;

  std::vector<phase_cb_t> startup_phase;
  std::vector<phase_cb_t> update_phase;
  std::vector<phase_cb_t> shutdown_phase;

  bool running;
};

}  // namespace wren
