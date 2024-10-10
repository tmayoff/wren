#include "wren/window.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL_error.h>
#include <SDL_mouse.h>

#include <tl/expected.hpp>
#include <tracy/Tracy.hpp>

#include "keycode.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_handles.hpp"
#include "wren/event.hpp"

namespace wren {

auto Window::create(const std::string &application_name) -> expected<Window> {
  ZoneScoped;
  spdlog::debug("Initializing window");
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_INIT));
  }

  SDL_Window *window =
      SDL_CreateWindow(application_name.c_str(), SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 640, 480,
                       SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
  if (window == nullptr) {
    spdlog::error("Failed to create window: {}", SDL_GetError());
    return tl::make_unexpected(make_error_code(WindowErrors::SDL_WINDOW));
  }

  return Window(window);
}

void Window::shutdown() { SDL_Quit(); }

auto Window::create_surface(const ::vk::Instance &instance)
    -> expected<::vk::SurfaceKHR> {
  VkSurfaceKHR surface{};
  SDL_Vulkan_CreateSurface(window_, instance, &surface);

  return surface;
}

auto Window::get_required_vulkan_extension() const
    -> expected<std::vector<std::string_view>> {
  uint32_t count = 0;
  bool res = SDL_Vulkan_GetInstanceExtensions(window_, &count, nullptr);
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  std::vector<const char *> extensions(count);
  res = SDL_Vulkan_GetInstanceExtensions(window_, &count, extensions.data());
  if (!res) {
    return tl::make_unexpected(
        make_error_code(WindowErrors::SDL_VULKAN_EXTENSION));
  }

  return std::vector<std::string_view>{extensions.begin(), extensions.end()};
}

auto sdl_mouse_to_wren_mouse(uint8_t sdl_button) {
  MouseCode code = MouseCode::Left;
  if (sdl_button == SDL_BUTTON_LEFT) return MouseCode::Left;
  if (sdl_button == SDL_BUTTON_MIDDLE) return code = MouseCode::Middle;
  if (sdl_button == SDL_BUTTON_RIGHT) return code = MouseCode::Right;

  return MouseCode::Middle;
}

auto sdl_key_to_wren_key(SDL_Keycode sdl_key) {
  switch (sdl_key) {
    case SDLK_UNKNOWN:
      return KeyCode::kUnknown;
    case SDLK_RETURN:
      return KeyCode::kEnter;
    case SDLK_ESCAPE:
      return KeyCode::kEscape;
    case SDLK_BACKSPACE:
      return KeyCode::kBackspace;
    case SDLK_TAB:
      return KeyCode::kTab;
    case SDLK_SPACE:
      return KeyCode::kSpace;
    case SDLK_EXCLAIM:
      return KeyCode::kExclaim;
    case SDLK_QUOTEDBL:
      return KeyCode::kDoubleQuote;
    case SDLK_HASH:
      return KeyCode::kHash;
    case SDLK_PERCENT:
      return KeyCode::kPercent;
    case SDLK_DOLLAR:
      return KeyCode::kDollar;
    case SDLK_AMPERSAND:
      return KeyCode::kAmpersand;
    case SDLK_QUOTE:
      return KeyCode::kQuote;
    case SDLK_LEFTPAREN:
      return KeyCode::kLeftParen;
    case SDLK_RIGHTPAREN:
      return KeyCode::kRightParen;
    case SDLK_ASTERISK:
      return KeyCode::kAsterix;
    case SDLK_PLUS:
      return KeyCode::kPlus;
    case SDLK_COMMA:
      return KeyCode::kComma;
    case SDLK_MINUS:
      return KeyCode::kMinus;
    case SDLK_PERIOD:
      return KeyCode::kPeriod;
    case SDLK_SLASH:
      return KeyCode::kSlash;
    case SDLK_0:
      return KeyCode::k0;
    case SDLK_1:
      return KeyCode::k1;
    case SDLK_2:
      return KeyCode::k2;
    case SDLK_3:
      return KeyCode::k3;
    case SDLK_4:
      return KeyCode::k4;
    case SDLK_5:
      return KeyCode::k5;
    case SDLK_6:
      return KeyCode::k6;
    case SDLK_7:
      return KeyCode::k7;
    case SDLK_8:
      return KeyCode::k8;
    case SDLK_9:
      return KeyCode::k9;
    case SDLK_COLON:
      return KeyCode::kColon;
    case SDLK_SEMICOLON:
      return KeyCode::kSemicolon;
    case SDLK_LESS:
      return KeyCode::kLess;
    case SDLK_EQUALS:
      return KeyCode::kEquals;
    case SDLK_GREATER:
      return KeyCode::kGreater;
    case SDLK_QUESTION:
      return KeyCode::kQuestion;
    case SDLK_AT:
      return KeyCode::kAt;
    case SDLK_LEFTBRACKET:
      return KeyCode::kLeftBracket;
    case SDLK_BACKSLASH:
      return KeyCode::kBackslash;
    case SDLK_RIGHTBRACKET:
      return KeyCode::kRightBracket;
    case SDLK_CARET:
      return KeyCode::kCaret;
    case SDLK_UNDERSCORE:
      return KeyCode::kUnderscore;
    case SDLK_BACKQUOTE:
      return KeyCode::kBackquote;
    case SDLK_a:
      return KeyCode::kA;
    case SDLK_b:
      return KeyCode::kB;
    case SDLK_c:
      return KeyCode::kC;
    case SDLK_d:
      return KeyCode::kD;
    case SDLK_e:
      return KeyCode::kE;
    case SDLK_f:
      return KeyCode::kF;
    case SDLK_g:
      return KeyCode::kG;
    case SDLK_h:
      return KeyCode::kH;
    case SDLK_i:
      return KeyCode::kI;
    case SDLK_j:
      return KeyCode::kJ;
    case SDLK_k:
      return KeyCode::kK;
    case SDLK_l:
      return KeyCode::kL;
    case SDLK_m:
      return KeyCode::kM;
    case SDLK_n:
      return KeyCode::kN;
    case SDLK_o:
      return KeyCode::kO;
    case SDLK_p:
      return KeyCode::kP;
    case SDLK_q:
      return KeyCode::kQ;
    case SDLK_r:
      return KeyCode::kR;
    case SDLK_s:
      return KeyCode::kS;
    case SDLK_t:
      return KeyCode::kT;
    case SDLK_u:
      return KeyCode::kU;
    case SDLK_v:
      return KeyCode::kV;
    case SDLK_w:
      return KeyCode::kW;
    case SDLK_x:
      return KeyCode::kX;
    case SDLK_y:
      return KeyCode::kY;
    case SDLK_z:
      return KeyCode::kZ;
    case SDLK_CAPSLOCK:
      return KeyCode::kCapslock;
    case SDLK_F1:
      return KeyCode::kF1;
    case SDLK_F2:
      return KeyCode::kF2;
    case SDLK_F3:
      return KeyCode::kF3;
    case SDLK_F4:
      return KeyCode::kF4;
    case SDLK_F5:
      return KeyCode::kF5;
    case SDLK_F6:
      return KeyCode::kF6;
    case SDLK_F7:
      return KeyCode::kF7;
    case SDLK_F8:
      return KeyCode::kF8;
    case SDLK_F9:
      return KeyCode::kF9;
    case SDLK_F10:
      return KeyCode::kF10;
    case SDLK_F11:
      return KeyCode::kF11;
    case SDLK_F12:
      return KeyCode::kF12;
    case SDLK_PRINTSCREEN:
      return KeyCode::kPrintScreen;
    case SDLK_SCROLLLOCK:
      return KeyCode::kScrollLock;
    case SDLK_PAUSE:
      return KeyCode::kPause;
    case SDLK_INSERT:
      return KeyCode::kInsert;
    case SDLK_HOME:
      return KeyCode::kHome;
    case SDLK_PAGEUP:
      return KeyCode::kPageUp;
    case SDLK_DELETE:
      return KeyCode::kDelete;
    case SDLK_END:
      return KeyCode::kEnd;
    case SDLK_PAGEDOWN:
      return KeyCode::kPageDown;
    case SDLK_RIGHT:
      return KeyCode::kRight;
    case SDLK_LEFT:
      return KeyCode::kLeft;
    case SDLK_DOWN:
      return KeyCode::kDown;
    case SDLK_UP:
      return KeyCode::kUp;
    case SDLK_NUMLOCKCLEAR:
    case SDLK_KP_DIVIDE:
    case SDLK_KP_MULTIPLY:
    case SDLK_KP_MINUS:
    case SDLK_KP_PLUS:
    case SDLK_KP_ENTER:
    case SDLK_KP_1:
    case SDLK_KP_2:
    case SDLK_KP_3:
    case SDLK_KP_4:
    case SDLK_KP_5:
    case SDLK_KP_6:
    case SDLK_KP_7:
    case SDLK_KP_8:
    case SDLK_KP_9:
    case SDLK_KP_0:
    case SDLK_KP_PERIOD:
    case SDLK_APPLICATION:
    case SDLK_POWER:
    case SDLK_KP_EQUALS:
    case SDLK_F13:
    case SDLK_F14:
    case SDLK_F15:
    case SDLK_F16:
    case SDLK_F17:
    case SDLK_F18:
    case SDLK_F19:
    case SDLK_F20:
    case SDLK_F21:
    case SDLK_F22:
    case SDLK_F23:
    case SDLK_F24:
    case SDLK_EXECUTE:
    case SDLK_HELP:
    case SDLK_MENU:
    case SDLK_SELECT:
    case SDLK_STOP:
    case SDLK_AGAIN:
    case SDLK_UNDO:
    case SDLK_CUT:
    case SDLK_COPY:
    case SDLK_PASTE:
    case SDLK_FIND:
    case SDLK_MUTE:
    case SDLK_VOLUMEUP:
    case SDLK_VOLUMEDOWN:
    case SDLK_KP_COMMA:
    case SDLK_KP_EQUALSAS400:
    case SDLK_ALTERASE:
    case SDLK_SYSREQ:
    case SDLK_CANCEL:
    case SDLK_CLEAR:
    case SDLK_PRIOR:
    case SDLK_RETURN2:
    case SDLK_SEPARATOR:
    case SDLK_OUT:
    case SDLK_OPER:
    case SDLK_CLEARAGAIN:
    case SDLK_CRSEL:
    case SDLK_EXSEL:
    case SDLK_KP_00:
    case SDLK_KP_000:
    case SDLK_THOUSANDSSEPARATOR:
    case SDLK_DECIMALSEPARATOR:
    case SDLK_CURRENCYUNIT:
    case SDLK_CURRENCYSUBUNIT:
    case SDLK_KP_LEFTPAREN:
    case SDLK_KP_RIGHTPAREN:
    case SDLK_KP_LEFTBRACE:
    case SDLK_KP_RIGHTBRACE:
    case SDLK_KP_TAB:
    case SDLK_KP_BACKSPACE:
    case SDLK_KP_A:
    case SDLK_KP_B:
    case SDLK_KP_C:
    case SDLK_KP_D:
    case SDLK_KP_E:
    case SDLK_KP_F:
    case SDLK_KP_XOR:
    case SDLK_KP_POWER:
    case SDLK_KP_PERCENT:
    case SDLK_KP_LESS:
    case SDLK_KP_GREATER:
    case SDLK_KP_AMPERSAND:
    case SDLK_KP_DBLAMPERSAND:
    case SDLK_KP_VERTICALBAR:
    case SDLK_KP_DBLVERTICALBAR:
    case SDLK_KP_COLON:
    case SDLK_KP_HASH:
    case SDLK_KP_SPACE:
    case SDLK_KP_AT:
    case SDLK_KP_EXCLAM:
    case SDLK_KP_MEMSTORE:
    case SDLK_KP_MEMRECALL:
    case SDLK_KP_MEMCLEAR:
    case SDLK_KP_MEMADD:
    case SDLK_KP_MEMSUBTRACT:
    case SDLK_KP_MEMMULTIPLY:
    case SDLK_KP_MEMDIVIDE:
    case SDLK_KP_PLUSMINUS:
    case SDLK_KP_CLEAR:
    case SDLK_KP_CLEARENTRY:
    case SDLK_KP_BINARY:
    case SDLK_KP_OCTAL:
    case SDLK_KP_DECIMAL:
    case SDLK_KP_HEXADECIMAL:
    case SDLK_LCTRL:
    case SDLK_LSHIFT:
    case SDLK_LALT:
    case SDLK_LGUI:
    case SDLK_RCTRL:
    case SDLK_RSHIFT:
    case SDLK_RALT:
    case SDLK_RGUI:
    case SDLK_MODE:
    case SDLK_AUDIONEXT:
    case SDLK_AUDIOPREV:
    case SDLK_AUDIOSTOP:
    case SDLK_AUDIOPLAY:
    case SDLK_AUDIOMUTE:
    case SDLK_MEDIASELECT:
    case SDLK_WWW:
    case SDLK_MAIL:
    case SDLK_CALCULATOR:
    case SDLK_COMPUTER:
    case SDLK_AC_SEARCH:
    case SDLK_AC_HOME:
    case SDLK_AC_BACK:
    case SDLK_AC_FORWARD:
    case SDLK_AC_STOP:
    case SDLK_AC_REFRESH:
    case SDLK_AC_BOOKMARKS:
    case SDLK_BRIGHTNESSDOWN:
    case SDLK_BRIGHTNESSUP:
    case SDLK_DISPLAYSWITCH:
    case SDLK_KBDILLUMTOGGLE:
    case SDLK_KBDILLUMDOWN:
    case SDLK_KBDILLUMUP:
    case SDLK_EJECT:
    case SDLK_SLEEP:
    case SDLK_APP1:
    case SDLK_APP2:
    case SDLK_AUDIOREWIND:
    case SDLK_AUDIOFASTFORWARD:
    case SDLK_SOFTLEFT:
    case SDLK_SOFTRIGHT:
    case SDLK_CALL:
    case SDLK_ENDCALL:
    default:
      return KeyCode::kUnknown;
  }
}

void Window::dispatch_events(const event::Dispatcher &dispatcher) {
  ZoneScoped;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT: {
        dispatcher.dispatch(event::WindowClose{});
        break;

        case SDL_WINDOWEVENT:
          switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
              dispatcher.dispatch(event::WindowResized{
                  {static_cast<float>(event.window.data1),
                   static_cast<float>(event.window.data2)}});
              break;
          }
          break;
        case SDL_MOUSEMOTION:
          dispatcher.dispatch(
              event::MouseMoved{static_cast<float>(event.motion.x),
                                static_cast<float>(event.motion.y)});

          dispatcher.dispatch(
              event::MouseMoved{static_cast<float>(event.motion.xrel),
                                static_cast<float>(event.motion.yrel), true});
          break;

        case SDL_MOUSEBUTTONDOWN: {
          dispatcher.dispatch(event::MouseButtonDown{
              sdl_mouse_to_wren_mouse(event.button.button)});
          break;
        }

        case SDL_MOUSEBUTTONUP: {
          dispatcher.dispatch(event::MouseButtonUp{
              sdl_mouse_to_wren_mouse(event.button.button)});
          break;
        }
        case SDL_MOUSEWHEEL: {
          dispatcher.dispatch(
              event::MouseWheel{static_cast<float>(event.wheel.x),
                                static_cast<float>(event.wheel.y)});

          break;
        }
        case SDL_TEXTINPUT: {
            dispatcher.dispatch(event::KeyTyped{event.text.text});
            break;
        }
        case SDL_KEYUP: {
          dispatcher.dispatch(
              event::KeyUp{sdl_key_to_wren_key(event.key.keysym.sym)});
          break;
        }
        case SDL_KEYDOWN: {
          dispatcher.dispatch(
              event::KeyDown{sdl_key_to_wren_key(event.key.keysym.sym)});
          break;
        }
      }
    }
  }
}

}  // namespace wren
