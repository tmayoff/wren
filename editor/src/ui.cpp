#include "ui.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <wren/context.hpp>
#include <wren/renderer.hpp>
#include <wren_vk/errors.hpp>

namespace editor::ui {

void check_result(VkResult res) {
  if (res != VK_SUCCESS) {
    throw std::runtime_error("Vulkan error");
  }
}

auto mouse_code_to_imgui(wren::MouseCode code) {
  switch (code) {
    case wren::MouseCode::Left:
      return ImGuiMouseButton_Left;
    case wren::MouseCode::Right:
      return ImGuiMouseButton_Right;
    case wren::MouseCode::Middle:
      return ImGuiMouseButton_Middle;
  }
}

auto key_code_to_imgui(wren::KeyCode code) {
  switch (code) {
    case wren::KeyCode::kUnknown:
      return ImGuiKey::ImGuiKey_None;
    case wren::KeyCode::kEnter:
      return ImGuiKey::ImGuiKey_Enter;
    case wren::KeyCode::kEscape:
      return ImGuiKey::ImGuiKey_Escape;
    case wren::KeyCode::kBackspace:
      return ImGuiKey::ImGuiKey_Backspace;
    case wren::KeyCode::kTab:
      return ImGuiKey::ImGuiKey_Tab;
    case wren::KeyCode::kSpace:
      return ImGuiKey::ImGuiKey_Space;
    case wren::KeyCode::kExclaim:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kDoubleQuote:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kHash:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kPercent:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kDollar:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kAmpersand:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kQuote:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kLeftParen:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kRightParen:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kAsterix:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kPlus:
    case wren::KeyCode::kComma:
    case wren::KeyCode::kMinus:
    case wren::KeyCode::kPeriod:
    case wren::KeyCode::kSlash:
    case wren::KeyCode::k0:
      return ImGuiKey::ImGuiKey_0;
    case wren::KeyCode::k1:
      return ImGuiKey::ImGuiKey_1;
    case wren::KeyCode::k2:
      return ImGuiKey::ImGuiKey_2;
    case wren::KeyCode::k3:
      return ImGuiKey::ImGuiKey_3;
    case wren::KeyCode::k4:
      return ImGuiKey::ImGuiKey_4;
    case wren::KeyCode::k5:
      return ImGuiKey::ImGuiKey_5;
    case wren::KeyCode::k6:
      return ImGuiKey::ImGuiKey_6;
    case wren::KeyCode::k7:
      return ImGuiKey::ImGuiKey_7;
    case wren::KeyCode::k8:
      return ImGuiKey::ImGuiKey_8;
    case wren::KeyCode::k9:
      return ImGuiKey::ImGuiKey_9;
    case wren::KeyCode::kColon:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kSemicolon:
      return ImGuiKey::ImGuiKey_Semicolon;
    case wren::KeyCode::kLess:
      return ImGuiKey::ImGuiKey_LeftShift;
    case wren::KeyCode::kEquals:
      return ImGuiKey::ImGuiKey_Equal;
    case wren::KeyCode::kGreater:
      return ImGuiKey::ImGuiKey_RightShift;
    case wren::KeyCode::kQuestion:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kAt:
      return ImGuiKey::ImGuiKey_None;  // FIXME
    case wren::KeyCode::kLeftBracket:
      return ImGuiKey::ImGuiKey_LeftBracket;
    case wren::KeyCode::kRightBracket:
      return ImGuiKey::ImGuiKey_RightBracket;
    case wren::KeyCode::kBackslash:
      return ImGuiKey::ImGuiKey_Backslash;
    case wren::KeyCode::kCaret:
      return ImGuiKey::ImGuiKey_GraveAccent;
    case wren::KeyCode::kUnderscore:
      return ImGuiKey::ImGuiKey_None;  // NOLINT
    case wren::KeyCode::kBackquote:
      return ImGuiKey::ImGuiKey_None;
    case wren::KeyCode::kA:
      return ImGuiKey::ImGuiKey_A;
    case wren::KeyCode::kB:
      return ImGuiKey::ImGuiKey_B;
    case wren::KeyCode::kC:
      return ImGuiKey::ImGuiKey_C;
    case wren::KeyCode::kD:
      return ImGuiKey::ImGuiKey_D;
    case wren::KeyCode::kE:
      return ImGuiKey::ImGuiKey_E;
    case wren::KeyCode::kF:
      return ImGuiKey::ImGuiKey_F;
    case wren::KeyCode::kG:
      return ImGuiKey::ImGuiKey_G;
    case wren::KeyCode::kH:
      return ImGuiKey::ImGuiKey_H;
    case wren::KeyCode::kI:
      return ImGuiKey::ImGuiKey_I;
    case wren::KeyCode::kJ:
      return ImGuiKey::ImGuiKey_J;
    case wren::KeyCode::kK:
      return ImGuiKey::ImGuiKey_K;
    case wren::KeyCode::kL:
      return ImGuiKey::ImGuiKey_L;
    case wren::KeyCode::kM:
      return ImGuiKey::ImGuiKey_M;
    case wren::KeyCode::kN:
      return ImGuiKey::ImGuiKey_N;
    case wren::KeyCode::kO:
      return ImGuiKey::ImGuiKey_O;
    case wren::KeyCode::kP:
      return ImGuiKey::ImGuiKey_P;
    case wren::KeyCode::kQ:
      return ImGuiKey::ImGuiKey_Q;
    case wren::KeyCode::kR:
      return ImGuiKey::ImGuiKey_R;
    case wren::KeyCode::kS:
      return ImGuiKey::ImGuiKey_S;
    case wren::KeyCode::kT:
      return ImGuiKey::ImGuiKey_T;
    case wren::KeyCode::kU:
      return ImGuiKey::ImGuiKey_U;
    case wren::KeyCode::kV:
      return ImGuiKey::ImGuiKey_V;
    case wren::KeyCode::kW:
      return ImGuiKey::ImGuiKey_W;
    case wren::KeyCode::kX:
      return ImGuiKey::ImGuiKey_X;
    case wren::KeyCode::kY:
      return ImGuiKey::ImGuiKey_Y;
    case wren::KeyCode::kZ:
      return ImGuiKey::ImGuiKey_Z;
    case wren::KeyCode::kCapslock:
      return ImGuiKey::ImGuiKey_CapsLock;
    case wren::KeyCode::kF1:
      return ImGuiKey::ImGuiKey_F1;
    case wren::KeyCode::kF2:
      return ImGuiKey::ImGuiKey_F2;
    case wren::KeyCode::kF3:
      return ImGuiKey::ImGuiKey_F3;
    case wren::KeyCode::kF4:
      return ImGuiKey::ImGuiKey_F4;
    case wren::KeyCode::kF5:
      return ImGuiKey::ImGuiKey_F5;
    case wren::KeyCode::kF6:
      return ImGuiKey::ImGuiKey_F6;
    case wren::KeyCode::kF7:
      return ImGuiKey::ImGuiKey_F7;
    case wren::KeyCode::kF8:
      return ImGuiKey::ImGuiKey_F8;
    case wren::KeyCode::kF9:
      return ImGuiKey::ImGuiKey_F9;
    case wren::KeyCode::kF10:
      return ImGuiKey::ImGuiKey_F10;
    case wren::KeyCode::kF11:
      return ImGuiKey::ImGuiKey_F11;
    case wren::KeyCode::kF12:
      return ImGuiKey::ImGuiKey_F12;
    case wren::KeyCode::kF13:
      return ImGuiKey::ImGuiKey_F13;
    case wren::KeyCode::kF14:
      return ImGuiKey::ImGuiKey_F14;
    case wren::KeyCode::kF15:
      return ImGuiKey::ImGuiKey_F15;
    case wren::KeyCode::kF16:
      return ImGuiKey::ImGuiKey_F16;
    case wren::KeyCode::kF17:
      return ImGuiKey::ImGuiKey_F17;
    case wren::KeyCode::kF18:
      return ImGuiKey::ImGuiKey_F18;
    case wren::KeyCode::kF19:
      return ImGuiKey::ImGuiKey_F19;
    case wren::KeyCode::kF20:
      return ImGuiKey::ImGuiKey_F20;
    case wren::KeyCode::kF21:
      return ImGuiKey::ImGuiKey_F21;
    case wren::KeyCode::kF22:
      return ImGuiKey::ImGuiKey_F22;
    case wren::KeyCode::kF23:
      return ImGuiKey::ImGuiKey_F23;
    case wren::KeyCode::kF24:
      return ImGuiKey::ImGuiKey_F24;
    case wren::KeyCode::kPrintScreen:
      return ImGuiKey::ImGuiKey_PrintScreen;
    case wren::KeyCode::kScrollLock:
      return ImGuiKey::ImGuiKey_ScrollLock;
    case wren::KeyCode::kPause:
      return ImGuiKey::ImGuiKey_Pause;
    case wren::KeyCode::kInsert:
      return ImGuiKey::ImGuiKey_Insert;
    case wren::KeyCode::kHome:
      return ImGuiKey::ImGuiKey_Home;
    case wren::KeyCode::kPageUp:
      return ImGuiKey::ImGuiKey_PageUp;
    case wren::KeyCode::kDelete:
      return ImGuiKey::ImGuiKey_Delete;
    case wren::KeyCode::kEnd:
      return ImGuiKey::ImGuiKey_End;
    case wren::KeyCode::kPageDown:
      return ImGuiKey::ImGuiKey_PageDown;
    case wren::KeyCode::kRight:
      return ImGuiKey::ImGuiKey_RightArrow;
    case wren::KeyCode::kLeft:
      return ImGuiKey::ImGuiKey_LeftArrow;
    case wren::KeyCode::kUp:
      return ImGuiKey::ImGuiKey_UpArrow;
    case wren::KeyCode::kDown:
      return ImGuiKey::ImGuiKey_DownArrow;
      break;
  }
}

auto init(const std::shared_ptr<wren::Context>& context)
    -> wren::expected<void> {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // ========== Setup events ===========
  context->event_dispatcher.on<wren::event::MouseMoved>([&io](const auto& e) {
    if (!e.relative) {
      io.AddMousePosEvent(e.x, e.y);
    } else {
      io.MouseDelta = {e.x, e.y};
    }
  });

  context->event_dispatcher.on<wren::event::MouseButtonDown>(
      [&io](const auto& e) {
        io.AddMouseButtonEvent(mouse_code_to_imgui(e.button), true);
      });

  context->event_dispatcher.on<wren::event::MouseButtonUp>(
      [&io](const auto& e) {
        io.AddMouseButtonEvent(mouse_code_to_imgui(e.button), false);
      });

  context->event_dispatcher.on<wren::event::KeyDown>(
      [&io](const auto& e) { io.AddKeyEvent(key_code_to_imgui(e.key), true); });

  context->event_dispatcher.on<wren::event::KeyUp>([&io](const auto& e) {
    io.AddKeyEvent(key_code_to_imgui(e.key), false);
  });

  context->event_dispatcher.on<wren::event::MouseWheel>(
      [&io](const auto& e) { io.AddMouseWheelEvent(e.x, e.y); });

  // Initialize rendering

  ImGui_ImplSDL2_InitForVulkan(context->window.native_handle());

  const auto& graphics_context = context->graphics_context;

  ::vk::DescriptorPool pool;

  {
    std::array pool_sizes = {
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eSampler, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eCombinedImageSampler,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eSampledImage, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageImage, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eUniformTexelBuffer,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageTexelBuffer,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eUniformBuffer, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageBuffer, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eUniformBufferDynamic,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageBufferDynamic,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eInputAttachment, 1000},
    };

    ::vk::DescriptorPoolCreateInfo pool_info{
        ::vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1000 * pool_sizes.size(), pool_sizes};

    VK_TIE_RESULT(
        pool, graphics_context->Device().get().createDescriptorPool(pool_info));
  }

  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.Instance = graphics_context->Instance();
  init_info.PhysicalDevice = graphics_context->PhysicalDevice();
  init_info.Device = graphics_context->Device().get();
  init_info.Queue = graphics_context->Device().get_graphics_queue();
  init_info.QueueFamily =
      graphics_context->FindQueueFamilyIndices()->graphics_index;
  init_info.MinImageCount = 2;
  init_info.ImageCount = context->renderer->swapchain_images_views().size();
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.RenderPass =
      context->renderer->get_graph().node_by_name("ui")->render_pass->get();
  init_info.DescriptorPool = pool;
  init_info.CheckVkResultFn = &check_result;

  if (!ImGui_ImplVulkan_Init(&init_info)) {
    spdlog::error("Failed to initialize vulkan for imgui");
  }

  spdlog::debug("Initialized ImGui");

  return {};
}

auto begin() -> void {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

auto end() -> void {}

void flush(const ::vk::CommandBuffer& cmd) {
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();

  // send draw data to command_buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
}

}  // namespace editor::ui
