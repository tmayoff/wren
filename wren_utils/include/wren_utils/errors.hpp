#pragma once

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <optional>
#include <system_error>
#include <tl/expected.hpp>

#include "enums.hpp"
#include "macros.hpp"

namespace wren {

class Err {
 public:
  template <typename T>
  Err(T error) : error_code_(make_error_code(error)) {}
  Err(std::error_code const& ec) : error_code_(ec) {}
  Err(int32_t ec, std::error_category const& e_cat)
      : error_code_(ec, e_cat) {}

  [[nodiscard]] auto error() const { return error_code_; }

  [[nodiscard]] auto message() const { return error_code_.message(); }

  [[nodiscard]] auto extra_msg() const { return extra_message_; }

 private:
  std::error_code error_code_;
  std::optional<std::string> extra_message_;
};

// struct error_code : public std::error_code {
//   error_code(int32_t ec, std::error_category const& cat)
//       : ::error_code(ec, cat) {}

//   error_code(std::error_code ec) : std::error_code(ec) {}

//   friend auto operator<<(std::ostream& os, error_code const& ec)
//       -> std::ostream& {
//     return os << ec.category().name() << " : " << ec.message();
//   }
// };

template <typename T>
using expected = tl::expected<T, Err>;

}  // namespace wren

template <>
struct fmt::formatter<wren::Err> : fmt::formatter<std::string> {
  auto format(wren::Err, fmt::format_context& ctx) const
      -> decltype(ctx.out());
};

// NOLINTNEXTLINE
#define ERROR_CODE(NAMESPACE, ERROR_ENUM)                           \
  namespace std {                                                   \
  template <>                                                       \
  struct is_error_code_enum<NAMESPACE::ERROR_ENUM> : true_type {};  \
  }                                                                 \
  namespace detail {                                                \
  class ERROR_ENUM##_category : public std::error_category {        \
   public:                                                          \
    [[nodiscard]] auto name() const noexcept -> const char* final { \
      return #ERROR_ENUM;                                           \
    }                                                               \
    [[nodiscard]] auto message(int32_t c) const                     \
        -> std::string final {                                      \
      NAMESPACE::ERROR_ENUM e =                                     \
          static_cast<NAMESPACE::ERROR_ENUM>(c);                    \
      return enum_to_string(e);                                     \
    }                                                               \
  };                                                                \
  }                                                                 \
  inline auto ERROR_ENUM##_category()                               \
      -> const detail::ERROR_ENUM##_category& {                     \
    static detail::ERROR_ENUM##_category c;                         \
    return c;                                                       \
  }                                                                 \
  inline auto make_error_code(NAMESPACE::ERROR_ENUM ec) {           \
    return wren::Err(static_cast<int32_t>(ec),                      \
                     ERROR_ENUM##_category());                      \
  }

// NOLINTNEXTLINE
#define ERROR_ENUM(NS, E, ...)   \
  namespace NS {                 \
  DESCRIBED_ENUM(E, __VA_ARGS__) \
  }                              \
  ERROR_CODE(NS, E)

///
/// @brief A rust-like ? operation that returns on an error
///
/// @param out A variable to save the resulting expected's value, on
/// success
/// @param err The expression resulting in an expected value
///
// NOLINTNEXTLINE
#define ERR_PROP(out, err)                                      \
  const auto LINEIZE(res, __LINE__) = err;                      \
  if (!LINEIZE(res, __LINE__).has_value())                      \
    return tl::make_unexpected(LINEIZE(res, __LINE__).error()); \
  out = LINEIZE(res, __LINE__).value();

///
/// @brief A rust-like ? operation that returns on an error
///
/// @param err The expression resulting in an expected value
///
// NOLINTNEXTLINE
#define ERR_PROP_VOID(err)                 \
  const auto LINEIZE(res, __LINE__) = err; \
  if (!LINEIZE(res, __LINE__).has_value()) \
    return tl::make_unexpected(LINEIZE(res, __LINE__).error());

///
/// @brief Checks a tl::expected for error and either sets the out
/// param or runs the on_err
///
/// @param out A variable to save the resulting expected's value, on
/// success
/// @param err The expression resulting in an expected value
/// @param on_err The expression to run if there's an error
///
// NOLINTNEXTLINE
#define ERR_VAL_OR(out, err, on_err)         \
  const auto LINEIZE(res, __LINE__) = err;   \
  if (!LINEIZE(res, __LINE__).has_value()) { \
    on_err                                   \
  }                                          \
  out = LINEIZE(res, __LINE__).value();

///
/// @brief Checks a tl::expected for error runs the on_err
///
/// @param err The expression resulting in an expected value
/// @param on_err The expression to run if there's an error
///
// NOLINTNEXTLINE
#define ERR_VOID_OR(err, on_err)             \
  const auto LINEIZE(res, __LINE__) = err;   \
  if (!LINEIZE(res, __LINE__).has_value()) { \
    on_err                                   \
  }
