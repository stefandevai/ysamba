#include "./vector.hpp"

#include <tuple>
#include <utility>

namespace dl
{
Vector2::Vector2(const Vector2& other) noexcept
{
  x = other.x;
  y = other.y;
}

Vector2::Vector2(Vector2&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
}

Vector2& Vector2::operator=(const Vector2& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2& Vector2::operator=(Vector2&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  return *this;
}

bool Vector2::operator==(const Vector2& rhs) const
{
  return (x == rhs.x) && (y == rhs.y);
}

bool Vector2::operator<(const Vector2& rhs) const
{
  return std::tie(x, y) < std::tie(rhs.x, rhs.y);
}

bool Vector2::operator<=(const Vector2& rhs) const
{
  return !(rhs < *this);
}

bool Vector2::operator>(const Vector2& rhs) const
{
  return rhs < *this;
}

bool Vector2::operator>=(const Vector2& rhs) const
{
  return !(*this < rhs);
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
  return Vector2{x + rhs.x, y + rhs.y};
}

Vector2& Vector2::operator+=(const Vector2& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
  return Vector2{x - rhs.x, y - rhs.y};
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vector2 Vector2::operator*(const Vector2& rhs) const
{
  return Vector2{x * rhs.x, y * rhs.y};
}

Vector2& Vector2::operator*=(const Vector2& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

Vector2 Vector2::operator/(const Vector2& rhs) const
{
  return Vector2{x / rhs.x, y / rhs.y};
}

Vector2& Vector2::operator/=(const Vector2& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

Vector2 Vector2::operator*(const double rhs) const
{
  return Vector2{x * rhs, y * rhs};
}

Vector2& Vector2::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector2 Vector2::operator/(const double rhs) const
{
  return Vector2{x / rhs, y / rhs};
}

Vector2& Vector2::operator/=(const double rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

Vector2::operator Vector2i() const
{
  return Vector2i{static_cast<int>(x), static_cast<int>(y)};
}

Vector2::operator Vector3i() const
{
  return Vector3i{static_cast<int>(x), static_cast<int>(y), 0};
}

Vector2::operator Vector3() const
{
  return Vector3{x, y, 0.0};
}

Vector2::operator Vector4d() const
{
  return Vector4d{x, y, 0.0, 0.0};
}

Vector2::operator Vector4i() const
{
  return Vector4i{static_cast<int>(x), static_cast<int>(y), 0, 0};
}

Vector2 operator*(double lhs, const Vector2& rhs)
{
  return rhs * lhs;
}

Vector2 operator/(double lhs, const Vector2& rhs)
{
  return rhs / lhs;
}

Vector2 Vector2::floor() const
{
  return Vector2{std::floor(x), std::floor(y)};
}

Vector2 Vector2::ceil() const
{
  return Vector2{std::ceil(x), std::ceil(y)};
}

Vector2 Vector2::round() const
{
  return Vector2{std::round(x), std::round(y)};
}

Vector2i::Vector2i(const Vector2i& other) noexcept
{
  x = other.x;
  y = other.y;
}

Vector2i::Vector2i(Vector2i&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
}

Vector2i& Vector2i::operator=(const Vector2i& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2i& Vector2i::operator=(Vector2i&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  return *this;
}

bool Vector2i::operator==(const Vector2i& rhs) const
{
  return (y == rhs.y) && (x == rhs.x);
}

bool Vector2i::operator!=(const Vector2i& rhs) const
{
  return !(*this == rhs);
}

bool Vector2i::operator<(const Vector2i& rhs) const
{
  return std::tie(x, y) < std::tie(rhs.x, rhs.y);
}

bool Vector2i::operator<=(const Vector2i& rhs) const
{
  return !(rhs < *this);
}

bool Vector2i::operator>(const Vector2i& rhs) const
{
  return rhs < *this;
}

bool Vector2i::operator>=(const Vector2i& rhs) const
{
  return !(*this < rhs);
}

Vector2i Vector2i::operator+(const Vector2i& rhs) const
{
  return Vector2i{x + rhs.x, y + rhs.y};
}

Vector2i& Vector2i::operator+=(const Vector2i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Vector2i Vector2i::operator-(const Vector2i& rhs) const
{
  return Vector2i{x - rhs.x, y - rhs.y};
}

Vector2i& Vector2i::operator-=(const Vector2i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vector2i Vector2i::operator*(const Vector2i& rhs) const
{
  return Vector2i{x * rhs.x, y * rhs.y};
}

Vector2i& Vector2i::operator*=(const Vector2i& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}

Vector2i Vector2i::operator/(const Vector2i& rhs) const
{
  return Vector2i{x / rhs.x, y / rhs.y};
}

Vector2i& Vector2i::operator/=(const Vector2i& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

Vector2i Vector2i::operator*(const int rhs) const
{
  return Vector2i{x * rhs, y * rhs};
}

Vector2i& Vector2i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector2i Vector2i::operator*(const double rhs) const
{
  return Vector2i{x * rhs, y * rhs};
}

Vector2i& Vector2i::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector2i Vector2i::operator/(const int rhs) const
{
  return Vector2i{x / rhs, y / rhs};
}

Vector2i& Vector2i::operator/=(const int rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

Vector2i::operator Vector2() const
{
  return Vector2{static_cast<double>(x), static_cast<double>(y)};
}

Vector2i::operator Vector3i() const
{
  return Vector3i{x, y, 0};
}

Vector2i::operator Vector3() const
{
  return Vector3{static_cast<double>(x), static_cast<double>(y), 0.0};
}

Vector2i::operator Vector4d() const
{
  return Vector4d{static_cast<double>(x), static_cast<double>(y), 0.0, 0.0};
}

Vector2i::operator Vector4i() const
{
  return Vector4i{x, y, 0, 0};
}

Vector2i operator*(int lhs, const Vector2i& rhs)
{
  return rhs * lhs;
}

Vector2i operator/(int lhs, const Vector2i& rhs)
{
  return rhs / lhs;
}

Vector3i::Vector3i(const Vector3i& other) noexcept
{
  x = other.x;
  y = other.y;
  z = other.z;
}

Vector3i::Vector3i(Vector3i&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
  std::swap(z, other.z);
}

Vector3i& Vector3i::operator=(const Vector3i& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3i& Vector3i::operator=(Vector3i&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  std::swap(z, rhs.z);
  return *this;
}

bool Vector3i::operator==(const Vector3i& rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector3i::operator<(const Vector3i& rhs) const
{
  return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
}

bool Vector3i::operator<=(const Vector3i& rhs) const
{
  return !(rhs < *this);
}

bool Vector3i::operator>(const Vector3i& rhs) const
{
  return rhs < *this;
}

bool Vector3i::operator>=(const Vector3i& rhs) const
{
  return !(*this < rhs);
}

Vector3i Vector3i::operator+(const Vector3i& rhs) const
{
  return Vector3i{x + rhs.x, y + rhs.y, z + rhs.z};
}

Vector3i& Vector3i::operator+=(const Vector3i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3i Vector3i::operator-(const Vector3i& rhs) const
{
  return Vector3i{x - rhs.x, y - rhs.y, z - rhs.z};
}

Vector3i& Vector3i::operator-=(const Vector3i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3i Vector3i::operator*(const Vector3i& rhs) const
{
  return Vector3i{x * rhs.x, y * rhs.y, z * rhs.z};
}

Vector3i& Vector3i::operator*=(const Vector3i& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

Vector3i Vector3i::operator/(const Vector3i& rhs) const
{
  return Vector3i{x / rhs.x, y / rhs.y, z / rhs.z};
}

Vector3i& Vector3i::operator/=(const Vector3i& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

Vector3i Vector3i::operator*(const int rhs) const
{
  return Vector3i{x * rhs, y * rhs, z * rhs};
}

Vector3i& Vector3i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector3i Vector3i::operator*(const double rhs) const
{
  return Vector3i{x * rhs, y * rhs, z * rhs};
}

Vector3i& Vector3i::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector3i Vector3i::operator/(const int rhs) const
{
  return Vector3i{x / rhs, y / rhs, z / rhs};
}

Vector3i& Vector3i::operator/=(const int rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Vector3i::operator Vector2() const
{
  return Vector2{static_cast<double>(x), static_cast<double>(y)};
}

Vector3i::operator Vector2i() const
{
  return Vector2i{x, y};
}

Vector3i::operator Vector3() const
{
  return Vector3{static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)};
}

Vector3i::operator Vector4d() const
{
  return Vector4d{static_cast<double>(x), static_cast<double>(y), static_cast<double>(z), 0.0};
}

Vector3i::operator Vector4i() const
{
  return Vector4i{x, y, z, 0};
}

Vector2i Vector3i::xy() const
{
  return Vector2i{x, y};
}

Vector3i operator*(int lhs, const Vector3i& rhs)
{
  return rhs * lhs;
}

Vector3i operator/(int lhs, const Vector3i& rhs)
{
  return rhs / lhs;
}

Vector3::Vector3(const Vector3& other) noexcept
{
  x = other.x;
  y = other.y;
  z = other.z;
}

Vector3::Vector3(Vector3&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
  std::swap(z, other.z);
}

Vector3& Vector3::operator=(const Vector3& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3& Vector3::operator=(Vector3&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  std::swap(z, rhs.z);
  return *this;
}

bool Vector3::operator==(const Vector3& rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector3::operator<(const Vector3& rhs) const
{
  return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z);
}

bool Vector3::operator<=(const Vector3& rhs) const
{
  return !(rhs < *this);
}

bool Vector3::operator>(const Vector3& rhs) const
{
  return rhs < *this;
}

bool Vector3::operator>=(const Vector3& rhs) const
{
  return !(*this < rhs);
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
  return Vector3{x + rhs.x, y + rhs.y, z + rhs.z};
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
  return Vector3{x - rhs.x, y - rhs.y, z - rhs.z};
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3 Vector3::operator*(const Vector3& rhs) const
{
  return Vector3{x * rhs.x, y * rhs.y, z * rhs.z};
}

Vector3& Vector3::operator*=(const Vector3& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  return *this;
}

Vector3 Vector3::operator/(const Vector3& rhs) const
{
  return Vector3{x / rhs.x, y / rhs.y, z / rhs.z};
}

Vector3& Vector3::operator/=(const Vector3& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  return *this;
}

Vector3 Vector3::operator*(const double rhs) const
{
  return Vector3{x * rhs, y * rhs, z * rhs};
}

Vector3& Vector3::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector3 Vector3::operator/(const double rhs) const
{
  return Vector3{x / rhs, y / rhs, z / rhs};
}

Vector3& Vector3::operator/=(const double rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  return *this;
}

Vector3::operator Vector2() const
{
  return Vector2{x, y};
}

Vector3::operator Vector2i() const
{
  return Vector2i{static_cast<int>(x), static_cast<int>(y)};
}

Vector3::operator Vector3i() const
{
  return Vector3i{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)};
}

Vector3::operator Vector4d() const
{
  return Vector4d{x, y, z, 0.0};
}

Vector3::operator Vector4i() const
{
  return Vector4i{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), 0};
}

Vector3 operator*(double lhs, const Vector3& rhs)
{
  return rhs * lhs;
}

Vector3 operator/(double lhs, const Vector3& rhs)
{
  return rhs / lhs;
}

Vector2 Vector3::xy() const
{
  return Vector2{x, y};
}

Vector3 Vector3::floor() const
{
  return Vector3{std::floor(x), std::floor(y), std::floor(z)};
}

Vector3 Vector3::ceil() const
{
  return Vector3{std::ceil(x), std::ceil(y), std::ceil(z)};
}

Vector3 Vector3::round() const
{
  return Vector3{std::round(x), std::round(y), std::round(z)};
}

Vector4d::Vector4d(const Vector4d& other) noexcept
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

Vector4d::Vector4d(Vector4d&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
  std::swap(z, other.z);
  std::swap(w, other.w);
}

Vector4d& Vector4d::operator=(const Vector4d& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

Vector4d& Vector4d::operator=(Vector4d&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  std::swap(z, rhs.z);
  std::swap(w, rhs.w);
  return *this;
}

Vector4d Vector4d::floor() const
{
  return Vector4d{std::floor(x), std::floor(y), std::floor(z), std::floor(w)};
}

Vector4d Vector4d::ceil() const
{
  return Vector4d{std::ceil(x), std::ceil(y), std::ceil(z), std::ceil(w)};
}

Vector4d Vector4d::round() const
{
  return Vector4d{std::round(x), std::round(y), std::round(z), std::round(w)};
}

bool Vector4d::operator==(const Vector4d& rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool Vector4d::operator<(const Vector4d& rhs) const
{
  return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}

bool Vector4d::operator<=(const Vector4d& rhs) const
{
  return !(rhs < *this);
}

bool Vector4d::operator>(const Vector4d& rhs) const
{
  return rhs < *this;
}

bool Vector4d::operator>=(const Vector4d& rhs) const
{
  return !(*this < rhs);
}

Vector4d Vector4d::operator+(const Vector4d& rhs) const
{
  return Vector4d{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
}

Vector4d& Vector4d::operator+=(const Vector4d& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vector4d Vector4d::operator-(const Vector4d& rhs) const
{
  return Vector4d{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
}

Vector4d& Vector4d::operator-=(const Vector4d& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

Vector4d Vector4d::operator*(const Vector4d& rhs) const
{
  return Vector4d{x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
}

Vector4d& Vector4d::operator*=(const Vector4d& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

Vector4d Vector4d::operator/(const Vector4d& rhs) const
{
  return Vector4d{x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w};
}

Vector4d& Vector4d::operator/=(const Vector4d& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  w /= rhs.w;
  return *this;
}

Vector4d Vector4d::operator*(const double rhs) const
{
  return Vector4d{x * rhs, y * rhs, z * rhs, w * rhs};
}

Vector4d& Vector4d::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vector4d Vector4d::operator/(const double rhs) const
{
  return Vector4d{x / rhs, y / rhs, z / rhs, w / rhs};
}

Vector4d& Vector4d::operator/=(const double rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

Vector4d::operator Vector2() const
{
  return Vector2{x, y};
}

Vector4d::operator Vector2i() const
{
  return Vector2i{static_cast<int>(x), static_cast<int>(y)};
}

Vector4d::operator Vector3() const
{
  return Vector3{x, y, z};
}

Vector4d::operator Vector3i() const
{
  return Vector3i{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)};
}

Vector4d::operator Vector4i() const
{
  return Vector4i{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z), static_cast<int>(w)};
}

Vector4d operator*(double lhs, const Vector4d& rhs)
{
  return rhs * lhs;
}

Vector4d operator/(double lhs, const Vector4d& rhs)
{
  return rhs / lhs;
}

Vector4i::Vector4i(const Vector4i& other) noexcept
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

Vector4i::Vector4i(Vector4i&& other) noexcept
{
  std::swap(x, other.x);
  std::swap(y, other.y);
  std::swap(z, other.z);
  std::swap(w, other.w);
}

Vector4i& Vector4i::operator=(const Vector4i& rhs) noexcept
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

Vector4i& Vector4i::operator=(Vector4i&& rhs) noexcept
{
  std::swap(x, rhs.x);
  std::swap(y, rhs.y);
  std::swap(z, rhs.z);
  std::swap(w, rhs.w);
  return *this;
}

bool Vector4i::operator==(const Vector4i& rhs) const
{
  return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool Vector4i::operator<(const Vector4i& rhs) const
{
  return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}

bool Vector4i::operator<=(const Vector4i& rhs) const
{
  return !(rhs < *this);
}

bool Vector4i::operator>(const Vector4i& rhs) const
{
  return rhs < *this;
}

bool Vector4i::operator>=(const Vector4i& rhs) const
{
  return !(*this < rhs);
}

Vector4i Vector4i::operator+(const Vector4i& rhs) const
{
  return Vector4i{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
}

Vector4i& Vector4i::operator+=(const Vector4i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vector4i Vector4i::operator-(const Vector4i& rhs) const
{
  return Vector4i{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
}

Vector4i& Vector4i::operator-=(const Vector4i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

Vector4i Vector4i::operator*(const Vector4i& rhs) const
{
  return Vector4i{x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
}

Vector4i& Vector4i::operator*=(const Vector4i& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  z *= rhs.z;
  w *= rhs.w;
  return *this;
}

Vector4i Vector4i::operator/(const Vector4i& rhs) const
{
  return Vector4i{x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w};
}

Vector4i& Vector4i::operator/=(const Vector4i& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  z /= rhs.z;
  w /= rhs.w;
  return *this;
}

Vector4i Vector4i::operator*(const int rhs) const
{
  return Vector4i{x * rhs, y * rhs, z * rhs, w * rhs};
}

Vector4i& Vector4i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vector4i Vector4i::operator*(const double rhs) const
{
  return Vector4i{x * rhs, y * rhs, z * rhs, w * rhs};
}

Vector4i& Vector4i::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vector4i Vector4i::operator/(const int rhs) const
{
  return Vector4i{x / rhs, y / rhs, z / rhs, w / rhs};
}

Vector4i& Vector4i::operator/=(const int rhs)
{
  x /= rhs;
  y /= rhs;
  z /= rhs;
  w /= rhs;
  return *this;
}

Vector4i::operator Vector2() const
{
  return Vector2{static_cast<double>(x), static_cast<double>(y)};
}

Vector4i::operator Vector2i() const
{
  return Vector2i{x, y};
}

Vector4i::operator Vector3() const
{
  return Vector3{static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)};
}

Vector4i::operator Vector3i() const
{
  return Vector3i{x, y, z};
}

Vector4i::operator Vector4d() const
{
  return Vector4d{static_cast<double>(x), static_cast<double>(y), static_cast<double>(z), static_cast<double>(w)};
}

Vector4i operator*(int lhs, const Vector4i& rhs)
{
  return rhs * lhs;
}

Vector4i operator/(int lhs, const Vector4i& rhs)
{
  return rhs / lhs;
}

}  // namespace dl
