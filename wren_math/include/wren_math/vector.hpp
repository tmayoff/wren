#pragma once

#include <array>
#include <cmath>

namespace wren::math {

template <typename T, std::size_t N>
struct Vec {
  using vec_t = Vec<T, N>;

  static auto UnitX() { return vec_t{1.0f}; }
  static auto UnitY() { return vec_t{0.0f, 1.0f}; }
  static auto UnitZ() { return vec_t{0.0f, 0.0f, 1.0f}; }

  Vec() : data() {}

  Vec(std::array<T, N> data) : data(data) {}

  // template <typename... Args>
  // Vec(Args&&... d) : data({{std::forward<Args>(d)...}}) {}

  auto at(std::size_t i) -> T& { return data.at(i); }
  [[nodiscard]] auto at(std::size_t i) const { return data.at(i); }

  constexpr auto operator*=(float scalar) const {
    for (auto& d : data) {
      d *= scalar;
    }
  }

  constexpr auto operator*(float scalar) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++) v.data.at(i) = data.at(i) * scalar;
    return v;
  }

  constexpr auto operator*=(const vec_t& other) {
    for (std::size_t i = 0; i < N; i++) {
      data.at(i) = data.at(i) * other.data.at(i);
    }
  }

  constexpr auto operator*(const vec_t& other) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++) {
      v.data.at(i) = data.at(i) * other.data.at(i);
    }
    return v;
  }

  [[nodiscard]] constexpr auto dot(const vec_t& other) const {
    T dot = 0;
    const auto a = this->normalized();
    const auto b = other.normalized();
    for (std::size_t i = 0; i < N; i++) dot += a.data.at(i) * b.data.at(i);
    return dot;
  }

  constexpr auto operator+=(const vec_t& other) {
    for (std::size_t i = 0; i < N; i++)
      data.at(i) = data.at(i) + other.data.at(i);
  }

  constexpr auto operator+(const vec_t& other) const {
    vec_t v{};
    for (std::size_t i = 0; i < N; i++)
      v.data.at(i) = data.at(i) + other.data.at(i);
    return v;
  }

  constexpr auto operator-(const vec_t& other) const {
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
    for (std::size_t i = 0; i < N; i++) v.data.at(i) = data.at(i) / scalar;
    return v;
  }

  constexpr auto operator==(const vec_t& other) const {
    return data == other.data;
  }

  constexpr auto operator!=(const vec_t& other) const {
    return !(*this == other);
  }

  [[nodiscard]] constexpr auto length() const {
    T sum = 0;
    for (const auto& d : data) sum += d * d;
    return std::sqrt(sum);
  }

  [[nodiscard]] auto normalized() const { return *this / length(); }

  std::array<T, N> data{};
};

struct vec2i : Vec<int, 2> {
  vec2i() : Vec<int, 2>() {}
  vec2i(int x, int y) : Vec<int, 2>({x, y}) {}
  vec2i(const Vec<int, 2>& other) : Vec<int, 2>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
};

struct vec2f : Vec<float, 2> {
  vec2f() : Vec<float, 2>() {}
  vec2f(float x, float y) : Vec<float, 2>({x, y}) {}
  vec2f(const Vec<float, 2>& other) : Vec<float, 2>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
};

struct Vec3f : Vec<float, 3> {
  Vec3f() : Vec<float, 3>() {}
  Vec3f(float x, float y, float z) : Vec<float, 3>({x, y, z}) {}
  Vec3f(const Vec<float, 3>& other) : Vec<float, 3>(other) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  auto x(float x) { data.at(0) = x; }
  [[nodiscard]] auto y() const { return data.at(1); }
  auto y(float y) { data.at(1) = y; }
  [[nodiscard]] auto z() const { return data.at(2); }
  auto z(float z) { data.at(2) = z; }

  auto operator%(const Vec3f& other) const {
    return Vec3f{y() * other.z() - z() * other.y(),
                 z() * other.x() - x() * other.z(),
                 x() * other.y() - y() * other.x()};
  }
};

struct Vec4f : Vec<float, 4> {
  Vec4f() : Vec<float, 4>() {}
  Vec4f(float x, float y, float z, float w) : Vec<float, 4>({x, y, z, w}) {}
  Vec4f(const Vec3f& vec, float w)
      : Vec<float, 4>({vec.x(), vec.y(), vec.z(), w}) {}
  Vec4f(const Vec<float, 4>& other) : Vec<float, 4>(other) {}
  Vec4f(std::array<float, 4> data) : Vec<float, 4>(data) {}

  [[nodiscard]] auto x() const { return data.at(0); }
  [[nodiscard]] auto y() const { return data.at(1); }
  [[nodiscard]] auto z() const { return data.at(2); }
  [[nodiscard]] auto w() const { return data.at(3); }
};

}  // namespace wren::math
