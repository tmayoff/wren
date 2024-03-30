#pragma once

#include <array>
#include <cmath>

namespace wrenm {

template <typename T, std::size_t N>
struct vec {
  using vec_t = vec<T, N>;

  static auto UnitX() { return vec_t{1.0f}; }
  static auto UnitY() { return vec_t{0.0f, 1.0f}; }
  static auto UnitZ() { return vec_t{0.0f, 0.0f, 1.0f}; }

  vec() : data() {}

  vec(auto&&... d) : data({{std::forward<decltype(d)>(d)...}}) {}
  vec(std::array<T, N> data) : data(data) {}

  constexpr auto operator*=(float scalar) const {
    for (auto& d : data) {
      d *= scalar;
    }
  }

  constexpr auto operator*(float scalar) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++)
      v.data.at(i) = data.at(i) * scalar;
    return v;
  }

  constexpr auto operator*=(vec_t const& other) {
    for (std::size_t i = 0; i < N; i++) {
      data.at(i) = data.at(i) * other.data.at(i);
    }
  }

  constexpr auto operator*(vec_t const& other) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++) {
      v.data.at(i) = data.at(i) * other.data.at(i);
    }
    return v;
  }

  [[nodiscard]] constexpr auto dot(vec_t const& other) const {
    T dot = 0;
    auto const a = this->normalized();
    auto const b = other.normalized();
    for (std::size_t i = 0; i < N; i++)
      dot += a.data.at(i) * b.data.at(i);
    return dot;
  }

  constexpr auto operator+=(vec_t const& other) {
    for (std::size_t i = 0; i < N; i++)
      data.at(i) = data.at(i) + other.data.at(i);
  }

  constexpr auto operator+(vec_t const& other) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++)
      v.data.at(i) = data.at(i) + other.data.at(i);
    return v;
  }

  constexpr auto operator-(vec_t const& other) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++)
      v.data.at(i) = data.at(i) - other.data.at(i);
    return v;
  }

  constexpr auto operator-() const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++) v.data.at(i) = -data.at(i);
    return v;
  }

  auto operator/(float scalar) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++)
      v.data.at(i) = data.at(i) / scalar;
    return v;
  }

  constexpr auto operator==(vec_t const& other) {
    return data == other.data;
  }

  constexpr auto operator!=(vec_t const& other) {
    return !(*this == other);
  }

  [[nodiscard]] constexpr auto length() const {
    T sum = 0;
    for (auto const& d : data) sum += d * d;
    return std::sqrt(sum);
  }

  [[nodiscard]] auto normalized() const { return *this / length(); }

  std::array<T, N> data{};
};

struct vec2f : vec<float, 2> {
  vec2f() : vec<float, 2>() {}
  vec2f(float x, float y) : vec<float, 2>(x, y) {}
  vec2f(vec<float, 2> const& other) : vec<float, 2>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
};

struct vec3f : vec<float, 3> {
  vec3f() : vec<float, 3>() {}
  vec3f(float x, float y, float z) : vec<float, 3>(x, y, z) {}
  vec3f(vec<float, 3> const& other) : vec<float, 3>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
  [[nodiscard]] auto z() const { return data.at(2); }

  auto operator%(vec3f const& other) const {
    return vec3f{y() * other.z() - z() * other.y(),
                 z() * other.x() - x() * other.z(),
                 x() * other.y() - y() * other.x()};
  }
};

struct vec4f : vec<float, 4> {
  vec4f() : vec<float, 4>() {}
  vec4f(float x, float y, float z, float w)
      : vec<float, 4>(x, y, z, w) {}
  vec4f(vec<float, 4> const& other) : vec<float, 4>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
  [[nodiscard]] auto z() const { return data.at(2); }
  [[nodiscard]] auto w() const { return data.at(3); }
};

}  // namespace wrenm
