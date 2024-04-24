#pragma once

#include <fmt/format.h>

#include <limits>

namespace dl
{
struct Vector2i;
struct Vector3;
struct Vector3i;
struct Vector4d;
struct Vector4i;

struct Vector2
{
  double x = 0.0;
  double y = 0.0;

  constexpr Vector2() = default;
  constexpr Vector2(double x, double y) noexcept : x(x), y(y) {}
  constexpr Vector2(double value) noexcept : x(value), y(value) {}
  constexpr Vector2(int x, int y) noexcept : x(static_cast<double>(x)), y(static_cast<double>(y)) {}

  ~Vector2() = default;
  Vector2(const Vector2& other) noexcept;
  Vector2(Vector2&& other) noexcept;

  Vector2& operator=(const Vector2& rhs) noexcept;
  Vector2& operator=(Vector2&& rhs) noexcept;
  bool operator==(const Vector2& rhs) const;
  bool operator<(const Vector2& rhs) const;
  bool operator<=(const Vector2& rhs) const;
  bool operator>(const Vector2& rhs) const;
  bool operator>=(const Vector2& rhs) const;
  Vector2 operator+(const Vector2& rhs) const;
  Vector2& operator+=(const Vector2& rhs);
  Vector2 operator-(const Vector2& rhs) const;
  Vector2& operator-=(const Vector2& rhs);
  Vector2 operator*(const Vector2& rhs) const;
  Vector2& operator*=(const Vector2& rhs);
  Vector2 operator/(const Vector2& rhs) const;
  Vector2& operator/=(const Vector2& rhs);
  Vector2 operator*(double rhs) const;
  Vector2& operator*=(double rhs);
  Vector2 operator/(double rhs) const;
  Vector2& operator/=(double rhs);

  explicit operator Vector2i() const;
  explicit operator Vector3i() const;
  explicit operator Vector3() const;
  explicit operator Vector4d() const;
  explicit operator Vector4i() const;

  Vector2 floor() const;
  Vector2 ceil() const;
  Vector2 round() const;

  static constexpr Vector2 null() { return Vector2{std::numeric_limits<double>::infinity()}; }
  static constexpr Vector2 zero() { return Vector2{0.0}; }
  static constexpr Vector2 one() { return Vector2{1.0}; }
};

Vector2 operator*(double lhs, const Vector2& rhs);
Vector2 operator/(double lhs, const Vector2& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector2& v)
{
  archive(v.x, v.y);
}

struct Vector2i
{
  int x = 0;
  int y = 0;

  constexpr Vector2i() = default;
  constexpr Vector2i(int x, int y) noexcept : x(x), y(y) {}
  constexpr Vector2i(int value) noexcept : x(value), y(value) {}
  constexpr Vector2i(double x, double y) noexcept : x(static_cast<int>(x)), y(static_cast<int>(y)) {}

  ~Vector2i() = default;
  Vector2i(const Vector2i& other) noexcept;
  Vector2i(Vector2i&& other) noexcept;

  Vector2i& operator=(const Vector2i& rhs) noexcept;
  Vector2i& operator=(Vector2i&& rhs) noexcept;
  bool operator==(const Vector2i& rhs) const;
  bool operator!=(const Vector2i& rhs) const;
  bool operator<(const Vector2i& rhs) const;
  bool operator<=(const Vector2i& rhs) const;
  bool operator>(const Vector2i& rhs) const;
  bool operator>=(const Vector2i& rhs) const;
  Vector2i operator+(const Vector2i& rhs) const;
  Vector2i& operator+=(const Vector2i& rhs);
  Vector2i operator-(const Vector2i& rhs) const;
  Vector2i& operator-=(const Vector2i& rhs);
  Vector2i operator*(const Vector2i& rhs) const;
  Vector2i& operator*=(const Vector2i& rhs);
  Vector2i operator/(const Vector2i& rhs) const;
  Vector2i& operator/=(const Vector2i& rhs);
  Vector2i operator*(int rhs) const;
  Vector2i& operator*=(int rhs);
  Vector2i operator*(double rhs) const;
  Vector2i& operator*=(double rhs);
  Vector2i operator/(int rhs) const;
  Vector2i& operator/=(int rhs);

  explicit operator Vector2() const;
  explicit operator Vector3i() const;
  explicit operator Vector3() const;
  explicit operator Vector4d() const;
  explicit operator Vector4i() const;

  static constexpr Vector2i null() { return Vector2i{std::numeric_limits<int>::infinity()}; }
  static constexpr Vector2i zero() { return Vector2i{0}; }
  static constexpr Vector2i one() { return Vector2i{1}; }
};

Vector2i operator*(int lhs, const Vector2i& rhs);
Vector2i operator/(int lhs, const Vector2i& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector2i& v)
{
  archive(v.x, v.y);
}

struct Vector3
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  constexpr Vector3() = default;
  constexpr Vector3(double x, double y, double z) noexcept : x(x), y(y), z(z) {}
  constexpr Vector3(double value) noexcept : x(value), y(value), z(value) {}
  constexpr Vector3(int x, int y, int z) noexcept
      : x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z))
  {
  }

  ~Vector3() = default;
  Vector3(const Vector3& other) noexcept;
  Vector3(Vector3&& other) noexcept;

  Vector3& operator=(const Vector3& rhs) noexcept;
  Vector3& operator=(Vector3&& rhs) noexcept;
  bool operator==(const Vector3& rhs) const;
  bool operator<(const Vector3& rhs) const;
  bool operator<=(const Vector3& rhs) const;
  bool operator>(const Vector3& rhs) const;
  bool operator>=(const Vector3& rhs) const;
  Vector3 operator+(const Vector3& rhs) const;
  Vector3& operator+=(const Vector3& rhs);
  Vector3 operator-(const Vector3& rhs) const;
  Vector3& operator-=(const Vector3& rhs);
  Vector3 operator*(const Vector3& rhs) const;
  Vector3& operator*=(const Vector3& rhs);
  Vector3 operator/(const Vector3& rhs) const;
  Vector3& operator/=(const Vector3& rhs);
  Vector3 operator*(double rhs) const;
  Vector3& operator*=(double rhs);
  Vector3 operator/(double rhs) const;
  Vector3& operator/=(double rhs);

  explicit operator Vector2() const;
  explicit operator Vector2i() const;
  explicit operator Vector3i() const;
  explicit operator Vector4d() const;
  explicit operator Vector4i() const;

  Vector2 xy() const;
  Vector3 floor() const;
  Vector3 ceil() const;
  Vector3 round() const;

  static constexpr Vector3 null() { return Vector3{std::numeric_limits<double>::infinity()}; }
  static constexpr Vector3 zero() { return Vector3{0.0}; }
  static constexpr Vector3 one() { return Vector3{1.0}; }
};

Vector3 operator*(double lhs, const Vector3& rhs);
Vector3 operator/(double lhs, const Vector3& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector3& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector3i
{
  int x = 0;
  int y = 0;
  int z = 0;

  constexpr Vector3i() = default;
  constexpr Vector3i(int x, int y, int z) noexcept : x(x), y(y), z(z) {}
  constexpr Vector3i(int value) noexcept : x(value), y(value), z(value) {}
  constexpr Vector3i(double x, double y, double z) noexcept
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
  {
  }

  ~Vector3i() = default;
  Vector3i(const Vector3i& other) noexcept;
  Vector3i(Vector3i&& other) noexcept;

  Vector3i& operator=(const Vector3i& rhs) noexcept;
  Vector3i& operator=(Vector3i&& rhs) noexcept;
  bool operator==(const Vector3i& rhs) const;
  bool operator<(const Vector3i& rhs) const;
  bool operator<=(const Vector3i& rhs) const;
  bool operator>(const Vector3i& rhs) const;
  bool operator>=(const Vector3i& rhs) const;
  Vector3i operator+(const Vector3i& rhs) const;
  Vector3i& operator+=(const Vector3i& rhs);
  Vector3i operator-(const Vector3i& rhs) const;
  Vector3i& operator-=(const Vector3i& rhs);
  Vector3i operator*(const Vector3i& rhs) const;
  Vector3i& operator*=(const Vector3i& rhs);
  Vector3i operator/(const Vector3i& rhs) const;
  Vector3i& operator/=(const Vector3i& rhs);
  Vector3i operator*(int rhs) const;
  Vector3i& operator*=(int rhs);
  Vector3i operator*(double rhs) const;
  Vector3i& operator*=(double rhs);
  Vector3i operator/(int rhs) const;
  Vector3i& operator/=(int rhs);

  explicit operator Vector2() const;
  explicit operator Vector2i() const;
  explicit operator Vector3() const;
  explicit operator Vector4d() const;
  explicit operator Vector4i() const;

  Vector2i xy() const;

  static constexpr Vector3i null() { return Vector3i{std::numeric_limits<int>::infinity()}; }
  static constexpr Vector3i zero() { return Vector3i{0}; }
  static constexpr Vector3i one() { return Vector3i{1}; }
};

Vector3i operator*(int lhs, const Vector3i& rhs);
Vector3i operator/(int lhs, const Vector3i& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector3i& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector4d
{
  double x = 0;
  double y = 0;
  double z = 0;
  double w = 0;

  constexpr Vector4d() = default;
  constexpr Vector4d(double x, double y, double z, double w) noexcept : x(x), y(y), z(z), w(w) {}
  constexpr Vector4d(double value) noexcept : x(value), y(value), z(value), w(value) {}
  constexpr Vector4d(int x, int y, int z, int w) noexcept
      : x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z)), w(static_cast<double>(w))
  {
  }

  ~Vector4d() = default;
  Vector4d(const Vector4d& other) noexcept;
  Vector4d(Vector4d&& other) noexcept;

  Vector4d& operator=(const Vector4d& rhs) noexcept;
  Vector4d& operator=(Vector4d&& rhs) noexcept;
  bool operator==(const Vector4d& rhs) const;
  bool operator<(const Vector4d& rhs) const;
  bool operator<=(const Vector4d& rhs) const;
  bool operator>(const Vector4d& rhs) const;
  bool operator>=(const Vector4d& rhs) const;
  Vector4d operator+(const Vector4d& rhs) const;
  Vector4d& operator+=(const Vector4d& rhs);
  Vector4d operator-(const Vector4d& rhs) const;
  Vector4d& operator-=(const Vector4d& rhs);
  Vector4d operator*(const Vector4d& rhs) const;
  Vector4d& operator*=(const Vector4d& rhs);
  Vector4d operator/(const Vector4d& rhs) const;
  Vector4d& operator/=(const Vector4d& rhs);
  Vector4d operator*(double rhs) const;
  Vector4d& operator*=(double rhs);
  Vector4d operator/(double rhs) const;
  Vector4d& operator/=(double rhs);

  explicit operator Vector2() const;
  explicit operator Vector2i() const;
  explicit operator Vector3() const;
  explicit operator Vector3i() const;
  explicit operator Vector4i() const;

  Vector4d floor() const;
  Vector4d ceil() const;
  Vector4d round() const;

  static constexpr Vector4d null() { return Vector4d{std::numeric_limits<double>::infinity()}; }
  static constexpr Vector4d zero() { return Vector4d{0.0}; }
  static constexpr Vector4d one() { return Vector4d{1.0}; }
};

Vector4d operator*(double lhs, const Vector4d& rhs);
Vector4d operator/(double lhs, const Vector4d& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector4d& v)
{
  archive(v.x, v.y, v.z, v.w);
}

struct Vector4i
{
  int x = 0;
  int y = 0;
  int z = 0;
  int w = 0;

  Vector4i() = default;
  constexpr Vector4i(int x, int y, int z, int w) noexcept : x(x), y(y), z(z), w(w) {}
  constexpr Vector4i(int value) noexcept : x(value), y(value), z(value), w(value) {}
  constexpr Vector4i(double x, double y, double z, double w) noexcept
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)), w(static_cast<int>(w))
  {
  }

  ~Vector4i() = default;
  Vector4i(const Vector4i& other) noexcept;
  Vector4i(Vector4i&& other) noexcept;

  Vector4i& operator=(const Vector4i& rhs) noexcept;
  Vector4i& operator=(Vector4i&& rhs) noexcept;
  bool operator==(const Vector4i& rhs) const;
  bool operator<(const Vector4i& rhs) const;
  bool operator<=(const Vector4i& rhs) const;
  bool operator>(const Vector4i& rhs) const;
  bool operator>=(const Vector4i& rhs) const;
  Vector4i operator+(const Vector4i& rhs) const;
  Vector4i& operator+=(const Vector4i& rhs);
  Vector4i operator-(const Vector4i& rhs) const;
  Vector4i& operator-=(const Vector4i& rhs);
  Vector4i operator*(const Vector4i& rhs) const;
  Vector4i& operator*=(const Vector4i& rhs);
  Vector4i operator/(const Vector4i& rhs) const;
  Vector4i& operator/=(const Vector4i& rhs);
  Vector4i operator*(int rhs) const;
  Vector4i& operator*=(int rhs);
  Vector4i operator*(double rhs) const;
  Vector4i& operator*=(double rhs);
  Vector4i operator/(int rhs) const;
  Vector4i& operator/=(int rhs);

  explicit operator Vector2() const;
  explicit operator Vector2i() const;
  explicit operator Vector3() const;
  explicit operator Vector3i() const;
  explicit operator Vector4d() const;

  static constexpr Vector4i null() { return Vector4i{std::numeric_limits<int>::infinity()}; }
  static constexpr Vector4i zero() { return Vector4i{0}; }
  static constexpr Vector4i one() { return Vector4i{1}; }
};

Vector4i operator*(int lhs, const Vector4i& rhs);
Vector4i operator/(int lhs, const Vector4i& rhs);

template <typename Archive>
void serialize(Archive& archive, Vector4i& v)
{
  archive(v.x, v.y, v.z, v.w);
}

}  // namespace dl

// Specialize fmt formatters
template <>
struct fmt::formatter<dl::Vector2>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector2& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {})", v.x, v.y);
  }
};

template <>
struct fmt::formatter<dl::Vector2i>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector2i& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {})", v.x, v.y);
  }
};

template <>
struct fmt::formatter<dl::Vector3>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector3& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
  }
};

template <>
struct fmt::formatter<dl::Vector3i>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector3i& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
  }
};

template <>
struct fmt::formatter<dl::Vector4d>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector4d& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
  }
};

template <>
struct fmt::formatter<dl::Vector4i>
{
  constexpr auto parse(format_parse_context& ctx) { return ctx.end(); }

  template <typename Context>
  auto format(const dl::Vector4i& v, Context& ctx)
  {
    return format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
  }
};
