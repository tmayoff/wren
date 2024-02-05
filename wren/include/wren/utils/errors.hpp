#pragma once

#include "enums.hpp"
#include <system_error>

#define ERROR_CODE(NAMESPACE, ERROR_ENUM, ...)                                 \
  namespace std {                                                              \
  template <> struct is_error_code_enum<NAMESPACE::ERROR_ENUM> : true_type {}; \
  }                                                                            \
  namespace detail {                                                           \
  class ERROR_ENUM##_category : public std::error_category {                   \
  public:                                                                      \
    [[nodiscard]] auto name() const noexcept -> const char * final {           \
      return #ERROR_ENUM;                                                      \
    }                                                                          \
    [[nodiscard]] auto message(int32_t c) const -> std::string final {         \
      NAMESPACE::ERROR_ENUM e = static_cast<NAMESPACE::ERROR_ENUM>(c);         \
      return enum_to_string(e);                                                \
    }                                                                          \
  };                                                                           \
  }                                                                            \
  inline auto ERROR_ENUM##_category()->const detail::ERROR_ENUM##_category & { \
    static detail::ERROR_ENUM##_category c;                                    \
    return c;                                                                  \
  }                                                                            \
  inline auto make_error_code(NAMESPACE::ERROR_ENUM ec)->std::error_code {     \
    return {static_cast<int32_t>(ec), ERROR_ENUM##_category()};                \
  }

#define ERROR_ENUM(NS, E, ...)                                                 \
  namespace NS {                                                               \
  DESCRIBED_ENUM(E, __VA_ARGS__)                                               \
  }                                                                            \
  ERROR_CODE(NS, E, __VA_ARGS__)
