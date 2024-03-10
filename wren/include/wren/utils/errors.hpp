#pragma once

#include <system_error>
#include <tl/expected.hpp>

#include "enums.hpp"
#include "macros.hpp"

namespace wren {
template <typename ValueType, typename ErrorType = std::error_code>
struct expected : public tl::expected<ValueType, ErrorType> {};

}  // namespace wren

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
      ->const detail::ERROR_ENUM##_category& {                      \
    static detail::ERROR_ENUM##_category c;                         \
    return c;                                                       \
  }                                                                 \
  inline auto make_error_code(NAMESPACE::ERROR_ENUM ec)             \
      ->std::error_code {                                           \
    return {static_cast<int32_t>(ec), ERROR_ENUM##_category()};     \
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
#define ERR_VOID_OR(err, on_err)        \
  const auto LINEIZE(res, __LINE__) = err;   \
  if (!LINEIZE(res, __LINE__).has_value()) { \
    on_err                                   \
  }
