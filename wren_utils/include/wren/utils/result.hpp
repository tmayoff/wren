#pragma once

#include <boost/describe.hpp>
#include <boost/preprocessor.hpp>
#include <expected>
#include <format>
#include <optional>
#include <system_error>

#include "enums.hpp"  // IWYU pragma: export
#include "macros.hpp"

namespace wren {

class Err {
 public:
  template <typename T>
  Err(T error) : error_code_(make_error_code(error)) {}
  Err(const std::error_code& ec) : error_code_(ec) {}
  Err(int32_t ec, const std::error_category& e_cat) : error_code_(ec, e_cat) {}

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
using expected = std::expected<T, Err>;

}  // namespace wren

template <>
struct std::formatter<wren::Err> : std::formatter<std::string> {
  auto format(wren::Err, std::format_context& ctx) const -> decltype(ctx.out());
};

//! @brief This macro creates the hooks into std::error_code for a
//! given error enum.
#define DEFINE_ERROR_IMPL(CAT_NAME, ERROR_ENUM)                            \
  class ERROR_ENUM##_category_t : public std::error_category {             \
   public:                                                                 \
    [[nodiscard]] auto name() const noexcept -> const char* final {        \
      return CAT_NAME;                                                     \
    }                                                                      \
    [[nodiscard]] auto message(int32_t c) const -> std::string final {     \
      auto e = static_cast<ERROR_ENUM>(c);                                 \
      using namespace boost::describe;                                     \
      return std::string{wren::utils::enum_to_string(e)};                  \
    }                                                                      \
  };                                                                       \
  inline auto ERROR_ENUM##_category()->const ERROR_ENUM##_category_t& {    \
    static const ERROR_ENUM##_category_t kC;                               \
    return kC;                                                             \
  }                                                                        \
  inline auto make_error_code(ERROR_ENUM ec) noexcept -> std::error_code { \
    return {static_cast<int32_t>(ec), ERROR_ENUM##_category()};            \
  }

/* @brief This macro defines an enum with BOOST_DEFINE_ENUM_CLASS and
 * hooks into the std::error_code system The NAMSPACE arg can be
 * skipped if the enum is in the global namespace.
 * @param cat_name The name to use for the Error's category, should
 * be something similar to the current namespace
 * @param name the error enum name
 * @param ... The enum variants to define
 */
#define DEFINE_ERROR(cat_name, name, ...)     \
  BOOST_DEFINE_ENUM_CLASS(name, __VA_ARGS__); \
  DEFINE_ERROR_IMPL(cat_name, name)

#define RESULT_UNIQUE_NAME() BOOST_PP_CAT(__result_tmp, __COUNTER__)

#define TRY_RESULT_IMPL(unique, expr) \
  auto unique = (expr);               \
  if (!(unique).has_value()) return std::unexpected(unique.error());

#define TRY_RESULT_1(unique, expr) TRY_RESULT_IMPL(unique, expr)
#define TRY_RESULT_2(unique, out, expr) \
  TRY_RESULT_IMPL(unique, expr)         \
  out = (unique).value();

#define TRY_RESULT(...)                       \
  BOOST_PP_OVERLOAD(TRY_RESULT_, __VA_ARGS__) \
  (RESULT_UNIQUE_NAME(), __VA_ARGS__)

#define TRY_RESULT_VOID_IMPL(unique, expr) \
  auto unique = (expr);                    \
  if (!(unique).has_value()) return;

#define TRY_RESULT_VOID_1(unique, expr) TRY_RESULT_VOID_IMPL(unique, expr)
#define TRY_RESULT_VOID_2(unique, out, expr) \
  TRY_RESULT_VOID_IMPL(unique, expr)         \
  out = (unique).value();
#define TRY_RESULT_VOID(...)                       \
  BOOST_PP_OVERLOAD(TRY_RESULT_VOID_, __VA_ARGS__) \
  (RESULT_UNIQUE_NAME(), __VA_ARGS__)

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
